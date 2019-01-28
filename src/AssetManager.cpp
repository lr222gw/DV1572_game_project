#include "AssetManager.h"

SharedPtr<Model> AssetManager::load_model(String const &filename) {
   return std::make_shared<Model>(filename);

}