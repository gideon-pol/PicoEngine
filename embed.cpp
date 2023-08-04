#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <lodepng.h>
#include "mathematics/vector.h"
#include "rendering/mesh.h"
#include "rendering/color.h"

FILE* open_or_exit(const char* fname, const char* mode)
{
    FILE* f = fopen(fname, mode);
    if (f == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }
    return f;
}

void convertOBJ(std::ifstream& in, std::ofstream& out, const char* sym){
    std::vector<vec3f> vertices;
    std::vector<vec3f> normals;
    std::vector<vec2f> uvs;

    std::vector<int> vertexIndices;
    std::vector<int> normalIndices;
    std::vector<int> uvIndices;

    std::string line;
    while(std::getline(in, line)){
        if(line[0] == 'v' && line[1] == ' '){
            vec3f vertex;
            sscanf(line.c_str(), "v %f %f %f", &vertex.data[0], &vertex.data[1], &vertex.data[2]);
            vertices.push_back(vertex);
        } else if(line[0] == 'v' && line[1] == 'n'){
            vec3f normal;
            sscanf(line.c_str(), "vn %f %f %f", &normal.data[0], &normal.data[1], &normal.data[2]);
            normals.push_back(normal);
        } else if(line[0] == 'v' && line[1] == 't'){
            vec2f uv;
            sscanf(line.c_str(), "vt %f %f", &uv.data[0], &uv.data[1]);
            uvs.push_back(uv);
        } else if(line[0] == 'f'){
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]
            );

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);

            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    std::vector<Vertex> bundledVertices;

    for(int i = 0; i < vertexIndices.size(); i++){
        Vertex vertex;
        vertex.Position = vertices.at(vertexIndices.at(i) - 1);
        vertex.Normal = normals.at(normalIndices.at(i) - 1);
        vertex.UV = uvs.at(uvIndices.at(i) - 1);
        bundledVertices.push_back(vertex);
    }

    // write the vertices to the file as a vertex array
    out << "#include \"rendering/mesh.h\"\n#include \"mathematics/vector.h\"\n\n";
    out << "Vertex " << sym  << "_vertices[" << bundledVertices.size() << "] = {\n";

    for(int i = 0; i < bundledVertices.size(); i++){
        out << "(Vertex){vec3f(" << bundledVertices[i].Position.x() << ", " << bundledVertices[i].Position.y() << ", " << bundledVertices[i].Position.z() << "),"
            << "vec3f(" << bundledVertices[i].Normal.x() << ", " << bundledVertices[i].Normal.y() << ", " << bundledVertices[i].Normal.z() << "),"
            << "vec2f(" << bundledVertices[i].UV.x() << ", " << bundledVertices[i].UV.y() << ")"
            << "}," << std::endl;
    }
    out.seekp(-2, std::ios_base::end);
    out << "};" << std::endl;

    // write the indices to the file as an index array.
    out << "uint32_t " << sym << "_indices[" << vertexIndices.size() << "] = {";

    // Simply write increasing numbers from 0 to the number of vertices
    for(int i = 0; i < vertexIndices.size(); i++){
        out << i << ", ";
    }
    out.seekp(-2, std::ios_base::end);

    out << "};" << std::endl;
}


void convertPNG(const char* path, std::ofstream& out, const char* sym){
    std::vector<unsigned char> image;
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, path);

    if(error){
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(1);
    }

    out << "#include \"rendering/texture.h\"\n#include \"rendering/color.h\"\n";
    out << "Color16 " << sym << "[" << width * height << "] = {\n" << std::endl;

    for(int i = 0; i < image.size(); i+=4){
        Color color = Color(image[i], image[i+1], image[i+2], image[i+3]);
        out << "0x" << std::hex << color.ToColor16() << ", ";
    }
    out.seekp(-2, std::ios_base::end);

    out << "};\n" << "const size_t " << sym << "_len = sizeof(" << sym << ");\n\n";
}

void outputAsBytes(std::ifstream& in, std::ofstream& out, const char* sym){
    out << "uint8_t " << sym << "[] = {\n" << std::endl;

    // read the file as binary and output the bytes
    char byte;
    while(in.read(&byte, 1)){
        out << "0x" << std::hex << (int)(unsigned char)byte << ", ";
    }

    // remove the last comma
    out.seekp(-2, std::ios_base::end);

    out << "};\n" << "const size_t " << sym << "_len = sizeof(" << sym << ");\n\n";

}

int main(int argc, char** argv)
{
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s {sym} {rsrc}\n\n"
            "  Creates {sym}.c from the contents of {rsrc}\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    const char* path = argv[1];
    const char* sym = argv[2];
    // FILE* in = open_or_exit(argv[3], "r");

    std::ifstream in(argv[3], std::ios::in | std::ios::binary);
    if(!in){
        std::cout << "Cannot open input file.\n";
        return 1;
    }

    char symfile[256];
    snprintf(symfile, sizeof(symfile), "%s/%s.cpp", path, sym);

    std::ofstream out(symfile, std::ios::out | std::ios::binary);
    if(!out){
        std::cout << "Cannot open output file.\n";
        return 1;
    }

    out << 
        "#include <stdlib.h>\n#include <stdint.h>\n" << std::endl;

    char* ext = strrchr(argv[3], '.');

    if (ext != NULL){
        if (strcmp(ext, ".obj") == 0){
            convertOBJ(in, out, sym);
        } else if(strcmp(ext, ".png") == 0){
            convertPNG(argv[3], out, sym);
        } else {
            outputAsBytes(in, out, sym);
        }
    } else {
        outputAsBytes(in, out, sym);
    }


    // unsigned char buf[256];
    // size_t nread = 0;
    // size_t linecount = 0;

    // do {
    //     nread = fread(buf, 1, sizeof(buf), in);
    //     size_t i;
    //     for (i=0; i < nread; i++) {
    //         fprintf(out, "0x%02x, ", buf[i]);
    //         if (++linecount == 10) { fprintf(out, "\n"); linecount = 0; }
    //     }
    // } while (nread > 0);

    // if (linecount > 0) fprintf(out, "\n");


    in.close();
    out.close();
    return EXIT_SUCCESS;
}