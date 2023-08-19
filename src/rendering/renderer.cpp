#include "rendering/renderer.h"

Camera Renderer::MainCamera = Camera(45fp, 0.1fp, 100fp, FRAME_WIDTH / FRAME_HEIGHT);
Color16 Renderer::FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
uint16_t Renderer::Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];

Camera::Camera(fixed fov, fixed near, fixed far, fixed aspect){
    this->fov = fov;
    this->near = near;
    this->far = far;

    projection = mat4f::perspective(fov, aspect, near, far);
    updateViewMatrix();
}

mat4f Camera::GetModelMatrix(){
    return mat4f::translate(position) * rotation.ToMatrix();
}

// Some internal caching. TODO: measure performance impact of this
mat4f& Camera::GetViewMatrix(){
    if(orientationUpdated){
        updateViewMatrix();
        orientationUpdated = false;
    }
    return view;
    // return GetModelMatrix().inverse();
}

// Frustum intersection test that checks if any of the bounding volume's corners
// are inside the frustrum. Very naive and stupid but SAT is expensive.
// This function can break for large bounding volumes or if the bounding volume
// is too close to the camera.
bool Camera::IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat){
    vec3f corners[8];
    volume.GetCorners(&corners);

    mat4f MVP = GetProjectionMatrix() * GetViewMatrix() * translationMat;
    for(int i = 0; i < 8; i++){
        vec3f corner = (MVP * vec4f(corners[i], 1)).homogenize();
        if( corner.x() >= -1 && corner.x() <= 1 &&
            corner.y() >= -1 && corner.y() <= 1 &&
            corner.z() >= 0 && corner.z() <= 1){
            return true;
        }
    };

    return false;
}

void Camera::updateViewMatrix(){
    view = rotation.ToMatrix() * mat4f::translate(-position);
    // view = GetModelMatrix().inverse();
}

void Renderer::Init(){
    bounds = BoundingBox2D(vec2f(0, 0), vec2f(FRAME_WIDTH, FRAME_HEIGHT));
    rasterizationMat = 
        mat4f::scale(vec3f(FRAME_WIDTH, FRAME_HEIGHT, 1)) *
        mat4f::translate(vec3f(0.5, 0.5, 0)) *
        mat4f::scale(vec3f(0.5, 0.5, 1));
}

void Renderer::Clear(Color color){
    for(int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++){
        FrameBuffer[i] = color.ToColor16();
        Zbuffer[i] = 65535;
    }
}

FORCE_INLINE void Renderer::PutPixel(vec2i16 pos, Color color){
    if(pos.x() >= 0 && pos.x() < FRAME_WIDTH && pos.y() >= 0 && pos.y() < FRAME_HEIGHT){
        FrameBuffer[pos.y() * FRAME_WIDTH + pos.x()] = color.ToColor16();
    }
}

void Renderer::DrawBox(BoundingBox2D box, Color color){
    BoundingBox2D bbi = bounds.Intersect(box);

    for(int y = SCAST<int>(bbi.Min.y()); y < SCAST<int>(bbi.Max.y()); y++){
        for(int x = SCAST<int>(bbi.Min.x()); x < SCAST<int>(bbi.Max.x()); x++){
            FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor16();
        }
    }
}

void Renderer::DrawBorder(BoundingBox2D box, uint8_t width, Color color){
    BoundingBox2D bbi = box;// bounds.Intersect(box);

    int32_t startX = SCAST<int32_t>(bbi.Min.x());
    int32_t startY = SCAST<int32_t>(bbi.Min.y());
    int32_t endX = SCAST<int32_t>(ceil(bbi.Max.x()));
    int32_t endY = SCAST<int32_t>(ceil(bbi.Max.y()));

    for(int y = startY; y < startY + width; y++){
        for(int x = startX; x < endX; x++){
            // FrameBuffer[y * FRAME_WIDTH + x] = color;
            PutPixel(vec2i16(x, y), color);
        }
    }

    for(int y = startY + width; y < endY - width; y++){
        for(int x = startX; x < startX + width; x++){
            // FrameBuffer[y * FRAME_WIDTH + x] = color;
            PutPixel(vec2i16(x, y), color);
        }

        for(int x = endX - width; x < endX; x++){
            // FrameBuffer[y * FRAME_WIDTH + x] = color;
            PutPixel(vec2i16(x, y), color);
        }
    }

    for(int y = endY - width; y < endY; y++){
        for(int x = startX; x < endX; x++){
            // FrameBuffer[y * FRAME_WIDTH + x] = color;
            PutPixel(vec2i16(x, y), color);

        }
    }
}

void Renderer::DrawLine(vec2i32 start, vec2i32 end, Color color, uint8_t lineWidth){
    int32_t x0 = start.x();
    int32_t y0 = start.y();
    int32_t x1 = end.x();
    int32_t y1 = end.y();

    int32_t dx = abs(x1 - x0);
    int32_t dy = abs(y1 - y0);

    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t sy = y0 < y1 ? 1 : -1;

    int32_t err = dx - dy;

    while(true){
        for(int y = y0 - lineWidth; y < y0 + lineWidth; y++){
            for(int x = x0 - lineWidth; x < x0 + lineWidth; x++){
                PutPixel(vec2i16(x, y), color);
            }
        }

        if(x0 == x1 && y0 == y1) break;

        int32_t e2 = 2 * err;

        if(e2 > -dy){
            err -= dy;
            x0 += sx;
        }

        if(e2 < dx){
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth){
    mat4f rVP = rasterizationMat * MainCamera.GetProjectionMatrix() * MainCamera.GetViewMatrix();

    vec3f v1 = (rVP * vec4f(p1, 1)).homogenize();
    vec3f v2 = (rVP * vec4f(p2, 1)).homogenize();

    DrawLine(v1.xy(), v2.xy(), color, lineWidth);
}

void Renderer::Blit(const Texture2D& tex, vec2i16 pos){
    BoundingBox2D bbi = BoundingBox2D(vec2i16(pos), vec2i16(tex.Width, tex.Height))
                        .Intersect(bounds);

    for(int y = SCAST<int16_t>(floor(bbi.Min.y())); y < SCAST<int16_t>(ceil(bbi.Max.y())); y++){
        for(int x = SCAST<int16_t>(floor(bbi.Min.x())); x < SCAST<int16_t>(ceil(bbi.Max.x())); x++){
            FrameBuffer[y * FRAME_WIDTH + x] = tex.GetPixel(vec2i16(x - pos.x(), y - pos.y())).ToColor16();
        }
    }
}

void Renderer::DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material){
    // TODO: do not calculate this for every mesh
    // printf("Checkpoint 1\n");
    mat4f rMVP = rasterizationMat *
                    MainCamera.GetProjectionMatrix() *
                    MainCamera.GetViewMatrix() * 
                    modelMat;

    // printf("Checkpoint 2\n");

    if(!MainCamera.IntersectsFrustrum(mesh.Volume, modelMat)){
        return;
    }

    // printf("Checkpoint 3\n");
    
    for(int i = 0; i < mesh.PolygonCount; i++){
        uint32_t idx = i * 3;

        TriangleShaderData t = {
            mesh.Vertices[mesh.Indices[idx]],
            mesh.Vertices[mesh.Indices[idx+1]],
            mesh.Vertices[mesh.Indices[idx+2]],
            Color::Purple
        };

        if(material._Shader.Type == ShaderType::Flat){
            t.TriangleColor = ((FlatShader::Parameters*)material.Parameters)->_Color;
        } else if(material._Shader.TriangleProgram){
            material._Shader.TriangleProgram(t, material.Parameters);
        }

        // printf("Checkpoint 4\n");

        vec3f pv1 = (rMVP * vec4f(t.v1.Position, 1)).homogenize();
        vec3f pv2 = (rMVP * vec4f(t.v2.Position, 1)).homogenize();
        vec3f pv3 = (rMVP * vec4f(t.v3.Position, 1)).homogenize();

        // printf("Checkpoint 5\n");

        BoundingBox2D bb = BoundingBox2D::FromTriangle(pv1.xy(), pv2.xy(), pv3.xy());
        BoundingBox2D bbi = bounds.Intersect(bb);

        if(bbi.IsEmpty()) continue;

#ifdef RENDER_DEBUG_TRIANGLE_BOUNDING
        DrawBorder(bbi, 1, Color::Yellow);
#endif

        fixed area;
        vec3f windingOrder = (pv2 - pv1).cross(pv3 - pv1);

        if(windingOrder.z() > 0) goto render_debug;

        area = edgeFunction(pv1, pv2, pv3);

        // printf("Checkpoint 6\n");

        for(int16_t x = SCAST<int16_t>(floor(bbi.Min.x())); x < SCAST<int16_t>(ceil(bbi.Max.x())); x++){
            for(int16_t y = SCAST<int16_t>(floor(bbi.Min.y())); y < SCAST<int16_t>(ceil(bbi.Max.y())); y++){
                vec3f p = vec3f(x, y, 0);
                fixed w0 = edgeFunction(pv2, pv3, p);
                fixed w1 = edgeFunction(pv3, pv1, p);
                fixed w2 = edgeFunction(pv1, pv2, p);

                if(w0 >= 0 && w1 >= 0 && w2 >= 0){
                    vec3f uvw = vec3f(w0, w1, w2) / area;
                    fixed z = vec3f(pv1.z(), pv2.z(), pv3.z()) * uvw;
                    if(z > 1.0f || z < 0.0f) continue;

                    // The precision of a fixed point is not good enough to multiply by 65535
                    // so we convert to float for the calculation
                    uint16_t z16 = SCAST<uint16_t>((float)z * 65535.0f);

                    if(z16 >= Zbuffer[y * FRAME_WIDTH + x]) continue;
                    Zbuffer[y * FRAME_WIDTH + x] = z16;

                    Color fragmentColor = t.TriangleColor;

                    switch(material._Shader.Type){
                        case ShaderType::Texture: {
                            TextureShader::Parameters* params = (TextureShader::Parameters*)material.Parameters;
                            Texture2D* tex = params->_Texture;
                            vec2f uv = (t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z());
                            uv = vec2f(uv.x() * params->TextureScale.x(), uv.y() * params->TextureScale.y());
                            fragmentColor = tex->Sample(uv).ToColor16();
                            break;
                        }
                        case ShaderType::Custom: {
                            if(material._Shader.FragmentProgram){
                                vec3f normal = t.v1.Normal * uvw.x() + t.v2.Normal * uvw.y() + t.v3.Normal * uvw.z();
                                normal = (modelMat * vec4f(normal, 0)).xyz().normalize();
                                vec3f fragCoord = vec3f(x, y, z);
                                vec2f uv = t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z();

                                FragmentShaderData data = {
                                    normal,
                                    fragCoord,
                                    uv,
                                    vec2f(FRAME_WIDTH, FRAME_HEIGHT),
                                    fragmentColor
                                };

                                material._Shader.FragmentProgram(data, material.Parameters);
                                fragmentColor = data.FragmentColor;
                            }
                            break;
                        }
                        default:
                            break;
                    }

                    FrameBuffer[y * FRAME_WIDTH + x] = fragmentColor.ToColor16();
                }
            }
        }

        // printf("Checkpoint 7\n");

        // TODO: this doesn't properly render because the drawing of other triangles
        //       will overwrite the debug drawings
        render_debug:
        #ifdef RENDER_DEBUG_WIREFRAME
            Color color = Color::Cyan; // ((WireFrameShader::Parameters*)material.Parameters)->_Color;
            vec2i32 p1 = vec2i16(SCAST<int32_t>(pv1.x()), SCAST<int32_t>(pv1.y()));
            vec2i32 p2 = vec2i16(SCAST<int32_t>(pv2.x()), SCAST<int32_t>(pv2.y()));
            vec2i32 p3 = vec2i16(SCAST<int32_t>(pv3.x()), SCAST<int32_t>(pv3.y()));

            DrawLine(p1, p2, color);
            DrawLine(p2, p3, color);
            DrawLine(p3, p1, color);
        #endif

        #ifdef RENDER_DEBUG_FACE_NORMALS
            vec3f pos = (t.v1.Position + t.v2.Position + t.v3.Position) / 3;
            pos = (modelMat * vec4f(pos, 1)).homogenize();

            vec3f normal = (t.v2.Position - t.v1.Position).cross(t.v3.Position - t.v1.Position).normalize();
            normal = (modelMat * vec4f(normal, 0)).xyz().normalize();

            Renderer::DrawLine(pos, pos + normal, Color::White);
        #endif

        // printf("Checkpoint 8\n");

        continue;
    }
}

void Renderer::Debug::DrawVolume(BoundingVolume& volume, mat4f& modelMat, Color color){
    vec3f corners[8];
    volume.GetCorners(&corners);

    for(int i = 0; i < 8; i++){
        corners[i] = (modelMat * vec4f(corners[i], 1)).xyz();
    }

    Renderer::DrawLine(corners[0], corners[1], color);
    Renderer::DrawLine(corners[0], corners[2], color);
    Renderer::DrawLine(corners[1], corners[3], color);
    Renderer::DrawLine(corners[2], corners[3], color);

    Renderer::DrawLine(corners[4], corners[5], color);
    Renderer::DrawLine(corners[4], corners[6], color);
    Renderer::DrawLine(corners[5], corners[7], color);
    Renderer::DrawLine(corners[6], corners[7], color);

    Renderer::DrawLine(corners[0], corners[4], color);
    Renderer::DrawLine(corners[1], corners[5], color);
    Renderer::DrawLine(corners[2], corners[6], color);
    Renderer::DrawLine(corners[3], corners[7], color);
}