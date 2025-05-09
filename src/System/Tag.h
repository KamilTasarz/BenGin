#pragma once

#include "../config.h"

class Tag //Gutten
{
public:
	std::string name;
	Tag(std::string name) : name(name) {}
	Tag() : name("") {}
	~Tag() = default;
	bool operator==(const Tag& other) const {
		return name == other.name;
	}
	bool operator!=(const Tag& other) const {
		return !(*this == other);
	}
};

class Layer
{
public:
	std::string name;
	Layer(std::string name) : name(name) {}
	Layer() : name("") {}
	~Layer() = default;
	bool operator==(const Layer& other) const {
		return name == other.name;
	}
	bool operator!=(const Layer& other) const {
		return !(*this == other);
	}
};

class TagLayerManager {
	std::vector<std::shared_ptr<Layer>> layers;
	std::vector<std::shared_ptr<Tag>> tags;
public:
	static TagLayerManager& Instance() {
		static TagLayerManager instance;
		return instance;
	}

	std::vector<std::shared_ptr<Layer>>& getLayers() {
		return layers;
	}

	std::vector<std::shared_ptr<Tag>>& getTags() {
		return tags;
	}

	void addLayer(std::string name) {
		layers.push_back(std::make_shared<Layer>(name));
	}
	void addTag(std::string name) {
		tags.push_back(std::make_shared<Tag>(name));
	}
	void removeLayer(std::string name) {
		layers.erase(std::remove_if(layers.begin(), layers.end(), [&](const std::shared_ptr<Layer>& layer) {
			return layer->name == name;
			}), layers.end());
	}
	void removeTag(std::string name) {
		tags.erase(std::remove_if(tags.begin(), tags.end(), [&](const std::shared_ptr<Tag>& tag) {
			return tag->name == name;
			}), tags.end());
	}

	std::shared_ptr<Layer> getLayer(std::string name) {
		for (auto& layer : layers) {
			if (layer->name == name) {
				return layer;
			}
		}
		return nullptr;
	}

	std::shared_ptr<Tag> getTag(std::string name) {
		for (auto& tag : tags) {
			if (tag->name == name) {
				return tag;
			}
		}
		return nullptr;
	}
};

