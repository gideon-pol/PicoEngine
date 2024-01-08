#include "rendering/renderer.h"

extern const uint8_t font_psf[];

Camera Renderer::MainCamera = Camera(45fp, 0.1fp, 500fp, FRAME_WIDTH / FRAME_HEIGHT);
Color565 Renderer::FrameBuffer[FRAME_WIDTH * FRAME_HEIGHT];
uint16_t Renderer::Zbuffer[FRAME_WIDTH * FRAME_HEIGHT];
Font Renderer::TextFont = Font((uint8_t*)&font_psf);
DepthTest Renderer::DepthTestMode = DepthTest::Less;
Culling Renderer::CullingMode = Culling::Back;

Color Renderer::ClearColor = Color::Black;

Camera::Camera(fixed fov, fixed near, fixed far, fixed aspect){
    this->fov = fov;
    this->near = near;
    this->far = far;

    projection = mat4f::perspective(fov, (float)aspect, (float)near, (float)far);
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
}

// Frustum intersection test that checks if any of the bounding volume's corners
// are inside the frustrum. Very naive and stupid but SAT is expensive.
// This function can break for large bounding volumes or if the bounding volume
// is too close to the camera.
bool Camera::IntersectsFrustrum(const BoundingVolume& volume, const mat4f& translationMat){
    vec3f corners[8];
    volume.GetCorners(&corners);

    // mat4f MVP = ((mat<float, 4, 4>)GetProjectionMatrix() * (mat<float, 4, 4>)GetViewMatrix() * (mat<float, 4, 4>)translationMat);
    mat4f MVP = GetProjectionMatrix() * GetViewMatrix() * translationMat;

    for(int i = 0; i < 8; i++){
        vec3f corner = (MVP * vec4f(corners[i], 1)).homogenize();
        if( corner.x() >= -1 && corner.x() <= 1 &&
            corner.y() >= -1 && corner.y() <= 1 &&
            corner.z() > 0 && corner.z() < 1){
            return true;
        }
    };

    return false;
}

void Camera::updateViewMatrix(){
    view = rotation.ToMatrix() * mat4f::translate(-position);
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
        FrameBuffer[i] = color.ToColor565();
        Zbuffer[i] = 65535;
    }
}

void Renderer::Prepare(){
    Clear(ClearColor);

    // Since this is run only once per frame, we do the calculations with floats instead of fixed
    // for better precision. Not doing so will lead to overflows during the multiplication.
    mat<float, 4, 4> vp = (mat<float, 4, 4>)MainCamera.GetProjectionMatrix() *
                          (mat<float, 4, 4>)MainCamera.GetViewMatrix();
    VP = vp;
    RVP = (mat<float, 4, 4>)rasterizationMat * vp;
}

void Renderer::DrawBox(BoundingBox2D box, Color color){
    BoundingBox2D bbi = bounds.Intersect(box);

    for(int y = SCAST<int>(bbi.Min.y()); y < SCAST<int>(bbi.Max.y()); y++){
        for(int x = SCAST<int>(bbi.Min.x()); x < SCAST<int>(bbi.Max.x()); x++){
            FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor565();
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

// 3D Bresenham that respects the camera's frustrum and depth buffer
void Renderer::DrawLine(vec3f p1, vec3f p2, Color color, uint8_t lineWidth){
    vec3f pv1 = (RVP * vec4f(p1, 1)).homogenize();
    vec3f pv2 = (RVP * vec4f(p2, 1)).homogenize();

    int32_t x0 = SCAST<int32_t>(pv1.x());
    int32_t y0 = SCAST<int32_t>(pv1.y());
    int32_t x1 = SCAST<int32_t>(pv2.x());
    int32_t y1 = SCAST<int32_t>(pv2.y());
    
    if((pv1.z() <= 0 || pv1.z() >= 1) || (pv2.z() <= 0 || pv2.z() >= 1)) return;

    int32_t z0 = SCAST<int32_t>((float)pv1.z() * 65535.0f);
    int32_t z1 = SCAST<int32_t>((float)pv2.z() * 65535.0f);

    int32_t dx = abs(x1 - x0);
    int32_t dy = abs(y1 - y0);
    int32_t dz = abs(z1 - z0);

    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t sz = z0 < z1 ? 1 : -1;

    if(dx >= dy && dx >= dz){
        int p1 = 2 * dy - dx;
        int p2 = 2 * dz - dx;

        while(x0 != x1){
            x0 += sx;
            if(p1 >= 0){
                y0 += sy;
                p1 -= 2 * dx;
            }
            if(p2 >= 0){
                z0 += sz;
                p2 -= 2 * dx;
            }
            p1 += 2 * dy;
            p2 += 2 * dz;

            if(z0 <= 0 || z0 >= 65535) continue;

            for(int y = y0 - lineWidth; y < y0 + lineWidth; y++){
                for(int x = x0 - lineWidth; x < x0 + lineWidth; x++){
                    if(x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT) continue;

                    if(testAndSetDepth(vec2i16(x, y), z0)){
                        FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor565();
                    }
                }
            }
        }
    } else if(dy >= dx && dy >= dz){
        int p1 = 2 * dx - dy;
        int p2 = 2 * dz - dy;

        while(y0 != y1){
            y0 += sy;
            if(p1 >= 0){
                x0 += sx;
                p1 -= 2 * dy;
            }
            if(p2 >= 0){
                z0 += sz;
                p2 -= 2 * dy;
            }
            p1 += 2 * dx;
            p2 += 2 * dz;

            if(z0 <= 0 || z0 >= 65535) continue;
            for(int y = y0 - lineWidth; y < y0 + lineWidth; y++){
                for(int x = x0 - lineWidth; x < x0 + lineWidth; x++){
                    if(x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT) continue;

                    if(testAndSetDepth(vec2i16(x, y), z0)){
                        FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor565();
                    }
                }
            }
        }
    } else if(dz >= dx && dz >= dy){
        int p1 = 2 * dy - dz;
        int p2 = 2 * dx - dz;

        while(z0 != z1){
            z0 += sz;
            if(p1 >= 0){
                y0 += sy;
                p1 -= 2 * dz;
            }
            if(p2 >= 0){
                x0 += sx;
                p2 -= 2 * dz;
            }
            p1 += 2 * dy;
            p2 += 2 * dx;

            if(z0 <= 0 || z0 >= 65535) continue;
            for(int y = y0 - lineWidth; y < y0 + lineWidth; y++){
                for(int x = x0 - lineWidth; x < x0 + lineWidth; x++){
                    if(x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT) continue;

                    if(testAndSetDepth(vec2i16(x, y), z0)){
                        FrameBuffer[y * FRAME_WIDTH + x] = color.ToColor565();
                    }
                }
            }
        }
    }
}

void Renderer::DrawText(const char* text, vec2i16 pos, Color color){
    int16_t x = pos.x();
    int16_t y = pos.y();

    for(int i = 0; text[i] != '\0'; i++){
        char c = text[i];
        if(c == '\n'){
            y += TextFont.GlyphSize.y();
            x = pos.x();
            continue;
        }

        const uint8_t* glyph = TextFont.GetGlyph(c);

        for(int gy = 0; gy < TextFont.GlyphSize.y(); gy++){
            int mask = 0b10000000;

            for(int gx = 0; gx < TextFont.GlyphSize.x(); gx++){
                if(*glyph & mask){
                    FrameBuffer[(y + gy) * FRAME_WIDTH + (x + gx)] = color.ToColor565();
                }
                mask >>= 1;
            }

            glyph += (TextFont.GlyphSize.x() + 7) / 8;
        }

        x += TextFont.GlyphSize.x();
    }
}

void Renderer::Blit(const Texture2D& tex, vec2i16 pos){
    BoundingBox2D bbi = BoundingBox2D(pos, pos + vec2i16(tex.Width, tex.Height))
                        .Intersect(bounds);

    for(int y = SCAST<int16_t>(floor(bbi.Min.y())); y < SCAST<int16_t>(ceil(bbi.Max.y())); y++){
        for(int x = SCAST<int16_t>(floor(bbi.Min.x())); x < SCAST<int16_t>(ceil(bbi.Max.x())); x++){
            FrameBuffer[y * FRAME_WIDTH + x] = tex.GetPixel(vec2i16(x - pos.x(), y - pos.y())).ToColor565();
        }
    }
}

void Renderer::DrawMesh(const Mesh& mesh, const mat4f& modelMat, const Material& material){
    if(!MainCamera.IntersectsFrustrum(mesh.Volume, modelMat)){
        return;
    }

    mat4f rMVP = RVP * modelMat;
    
    for(int i = 0; i < mesh.PolygonCount; i++){
        uint32_t idx = i * 3;

        TriangleShaderData t = {
            modelMat,
            mesh.Vertices[mesh.Indices[idx]],
            mesh.Vertices[mesh.Indices[idx+1]],
            mesh.Vertices[mesh.Indices[idx+2]],
            Color::Purple
        };

        Time::Profiler::Enter("TriangleProgram");
        if(material._Shader.Type == ShaderType::Flat){
            t.TriangleColor = ((FlatShader::Parameters*)material.Parameters)->_Color;
        } else if(material._Shader.TriangleProgram){
            material._Shader.TriangleProgram(t, material.Parameters);
        }
        Time::Profiler::Exit("TriangleProgram");

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

        int A01, A12, A20, B01, B12, B20;
        int w1_row, w2_row, w3_row;
        vec2<int> min;

        vec3<int> v1 = pv1;
        vec3<int> v2 = pv2;
        vec3<int> v3 = pv3;

        switch(CullingMode){
            case Culling::None:
                break;
            case Culling::Front:
                if(windingOrder.z() < 0) goto render_debug;
                break;
            case Culling::Back:
                if(windingOrder.z() > 0) goto render_debug;
                break;
        }

        area = edgeFunctionFast(v1.xy(), v2.xy(), v3.xy());

        if(area == 0) continue;

        A01 = v2.y() - v1.y(); B01 = v1.x() - v2.x();
        A12 = v3.y() - v2.y(); B12 = v2.x() - v3.x();
        A20 = v1.y() - v3.y(); B20 = v3.x() - v1.x();

        min = vec2<int>(SCAST<int>(floor(bbi.Min.x())), SCAST<int>(floor(bbi.Min.y())));

        w1_row = edgeFunctionFast(v2.xy(), v3.xy(), min);
        w2_row = edgeFunctionFast(v3.xy(), v1.xy(), min);
        w3_row = edgeFunctionFast(v1.xy(), v2.xy(), min);

        Time::Profiler::Enter("Rasterization");
        for(int16_t y = SCAST<int16_t>(floor(bbi.Min.y())); y < SCAST<int16_t>(ceil(bbi.Max.y())); y++){
            int w1 = w1_row;
            int w2 = w2_row;
            int w3 = w3_row;

            for(int16_t x = SCAST<int16_t>(floor(bbi.Min.x())); x < SCAST<int16_t>(ceil(bbi.Max.x())); x++){
                if((w1 | w2 | w3) >= 0){
                    vec3f uvw = vec3f(w1, w2, w3) / area;
                    fixed z = vec3f(pv1.z(), pv2.z(), pv3.z()) * uvw;

                    Color fragmentColor = t.TriangleColor;
                    uint16_t z16;

                    if(z >= 1.0f || z <= 0.0f) goto update_baricentric;

                    // The precision of a fixed point is not good enough to multiply by 65535
                    // so we convert to float for the calculation
                    z16 = SCAST<uint16_t>((float)z * 65535.0f);

                    if(!testAndSetDepth(vec2i16(x, y), z16)) goto update_baricentric;

                    switch(material._Shader.Type){
                        case ShaderType::Texture: {
                            TextureShader::Parameters* params = (TextureShader::Parameters*)material.Parameters;
                            Texture2D* tex = params->_Texture;
                            vec2f uv = (t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z());
                            uv = vec2f(uv.x() * params->TextureScale.x(), uv.y() * params->TextureScale.y());
                            fragmentColor = tex->Sample(uv);
                            break;
                        }
                        case ShaderType::Custom: {
                            if(material._Shader.FragmentProgram){
                                vec3f normal = t.v1.Normal * uvw.x() + t.v2.Normal * uvw.y() + t.v3.Normal * uvw.z();
                                normal = (modelMat * vec4f(normal, 0)).xyz().normalize();
                                vec3f pos = t.v1.Position * uvw.x() + t.v2.Position * uvw.y() + t.v3.Position * uvw.z();
                                pos = (modelMat * vec4f(pos, 1)).homogenize();
                                vec3f fragCoord = vec3f(x, y, z);
                                vec2f uv = t.v1.UV * uvw.x() + t.v2.UV * uvw.y() + t.v3.UV * uvw.z();

                                FragmentShaderData data = {
                                    pos,
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

                    FrameBuffer[y * FRAME_WIDTH + x] = fragmentColor.ToColor565();
                }

                update_baricentric:
                w1 += A12;
                w2 += A20;
                w3 += A01;
            }

            w1_row += B12;
            w2_row += B20;
            w3_row += B01;
        }

        Time::Profiler::Exit("Rasterization");

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

vec3f Renderer::WorldToScreen(vec3f worldPos){
    return (RVP * vec4f(worldPos, 1)).homogenize();
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

void Renderer::Debug::DrawOrientation(mat4f& modelMat){
    vec3f pos = modelMat.col(3).xyz();
    vec3f forward = modelMat.col(2).xyz();
    vec3f up = modelMat.col(1).xyz();
    vec3f right = modelMat.col(0).xyz();

    Renderer::DepthTestMode = DepthTest::Never;
    Renderer::DrawLine(pos, pos + forward, Color::White);
    Renderer::DrawLine(pos, pos + up, Color::Green);
    Renderer::DrawLine(pos, pos + right, Color::Red);
    Renderer::DepthTestMode = DepthTest::Less;
}