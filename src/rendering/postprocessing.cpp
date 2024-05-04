#include "rendering/postprocessing.h"

void PostProcessing::Add(Layer* layer) {
    layers.push_back(layer);
}

void PostProcessing::Remove(Layer* layer) {
    for (int i = 0; i < layers.size(); i++) {
        if (layers[i] == layer) {
            layers.erase(layers.begin() + i);
            return;
        }
    }
}

void PostProcessing::Apply(Color565* buffer, const vec2i16& size) {
    for (Layer* layer : layers) {
        layer->Apply(buffer, size);
    }
}