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
            float data[3];
            sscanf(line.c_str(), "v %f %f %f", &data[0], &data[1], &data[2]);
            vertices.push_back(vec3f(fixed(data[0]), fixed(data[1]), fixed(data[2])));
        } else if(line[0] == 'v' && line[1] == 'n'){
            float data[3];
            sscanf(line.c_str(), "vn %f %f %f", &data[0], &data[1], &data[2]);
            normals.push_back(vec3f(fixed(data[0]), fixed(data[1]), fixed(data[2])));
        } else if(line[0] == 'v' && line[1] == 't'){
            float data[2];
            sscanf(line.c_str(), "vt %f %f", &data[0], &data[1]);
            uvs.push_back(vec2f(fixed(data[0]), fixed(data[1])));
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
    out << "extern const Vertex " << sym  << "_vertices[" << bundledVertices.size() << "] = {\n";

    for(int i = 0; i < bundledVertices.size(); i++){
        out << "(Vertex){vec3f(" << bundledVertices[i].Position.x() << ", " << bundledVertices[i].Position.y() << ", " << bundledVertices[i].Position.z() << "),"
            << "vec3f(" << bundledVertices[i].Normal.x() << ", " << bundledVertices[i].Normal.y() << ", " << bundledVertices[i].Normal.z() << "),"
            << "vec2f(" << bundledVertices[i].UV.x() << ", " << bundledVertices[i].UV.y() << ")"
            << "}," << std::endl;
    }
    out.seekp(-2, std::ios_base::end);
    out << "};" << std::endl;

    // write the indices to the file as an index array.
    out << "extern const uint32_t " << sym << "_indices[" << vertexIndices.size() << "] = {";

    // Simply write increasing numbers from 0 to the number of vertices
    for(int i = 0; i < vertexIndices.size(); i++){
        out << i << ", ";
    }
    out.seekp(-2, std::ios_base::end);

    out << "};" << std::endl;
}

void convertPNG(const char* path, std::ofstream& fout, const char* sym){
    std::vector<unsigned char> image;
    unsigned width, height;

    unsigned error = lodepng::decode(image, width, height, path);

    if(error){
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(1);
    }

    fout << "#include \"rendering/texture.h\"\n#include \"rendering/color.h\"\n\n";
    fout << "extern const Color16 " << sym << "[" << width * height << "] = {\n";

    for(int i = 0; i < image.size(); i+=4){
        Color color = Color(image[i], image[i+1], image[i+2], image[i+3]);
        // Color332 color332 = color.ToColor332();
        Color16 color16 = color.ToColor16();
        fout << "0x" << std::hex << color16 << ", ";
    }
    fout.seekp(-2, std::ios_base::end);

    fout << "};\n" << "extern const size_t " << sym << "_len = sizeof(" << sym << ");\n\n";
}

void outputAsBytes(std::ifstream& in, std::ofstream& out, const char* sym){
    out << "extern const uint8_t " << sym << "[] = {\n" << std::endl;

    char byte;
    while(in.read(&byte, 1)){
        out << "0x" << std::hex << (int)(unsigned char)byte << ", ";
    }

    out.seekp(-2, std::ios_base::end);

    out << "};\n" << "extern const size_t " << sym << "_len = sizeof(" << sym << ");\n\n";

}

int main(int argc, char** argv)
{
    std::cout << "Started embedding procedure" << std::endl;
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
        fprintf(stderr, "Arg: %s\n", argv[3]);
        return 1;
    }

    std::cout << "Converting " << argv[3] << " to embeddable array" << std::endl;

    char symfile[256];
    snprintf(symfile, sizeof(symfile), "%s.cpp", sym);

    std::cout << "Writing to " << symfile << std::endl;
    std::cout << "Writing to " << path << std::endl;

    std::ofstream fout(path, std::ios::out | std::ios::binary);
    if(!fout){
        std::cout << "Cannot open output file.\n";
        return 1;
    }

    fout << 
        "#include <stdlib.h>\n#include <stdint.h>\n" << std::endl;

    char* ext = strrchr(argv[3], '.');

    if (ext != NULL){
        if (strcmp(ext, ".obj") == 0){
            convertOBJ(in, fout, sym);
        } else if(strcmp(ext, ".png") == 0){
            convertPNG(argv[3], fout, sym);
        } else {
            outputAsBytes(in, fout, sym);
        }
    } else {
        outputAsBytes(in, fout, sym);
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
    fout.close();
    return EXIT_SUCCESS;
}