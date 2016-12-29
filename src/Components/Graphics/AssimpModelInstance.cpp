/*
* Dream::Asset::Model::AssimpModelInstance
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "AssimpModelInstance.h"
#include "TextureCache.h"

namespace Dream {

    map<string,const aiScene*> AssimpModelInstance::AssimpModelCache = map<string,const aiScene*>();

    ::Assimp::Importer AssimpModelInstance::mImporter;
    const aiScene* AssimpModelInstance::getModelFromCache(string path) {
        map<string,const aiScene*>::iterator it;
        for (it=AssimpModelCache.begin();it!=AssimpModelCache.end();it++) {
            if ((*it).first.compare(path) == 0) {
                if (DEBUG) {
                    cout << "AssimpModelInstance: Found cached scene for " << path << endl;
                }
                return (*it).second;
            }
        }
        if (DEBUG) {
          cout << "AssimpModelInstance: Loading " << path << " from disk" << endl;
        }
        const aiScene* scene = mImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            cerr << "AssimpModelInstance: Error " << mImporter.GetErrorString() << endl;
            return nullptr;
        }
        AssimpModelCache.insert(pair<string,const aiScene*>(path,scene));
        return scene;
    }

    AssimpModelInstance::AssimpModelInstance(
            AssetDefinition* definition, Transform3D* transform
            ) : AssetInstance(definition,transform) {
        return;
    }

    AssimpModelInstance::~AssimpModelInstance() {
        return;
    }

    bool AssimpModelInstance::load(string projectPath) {
        string path = projectPath+mDefinition->getAssetPath();
        if (DEBUG) {
            cout << "AssimpModelInstance: Loading Model - " << path << endl;
        }
        const aiScene* scene = getModelFromCache(path);
        if(scene == nullptr){
            return false;
        }
        mDirectory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
        mLoaded = true;
        return mLoaded;
    }

    void AssimpModelInstance::draw(ShaderInstance* shader) {
        size_t nMeshes = mMeshes.size();
        for(size_t i = 0; i < nMeshes; i++ ) {
            mMeshes[i].draw(shader);
        }
    }

    void AssimpModelInstance::processNode(aiNode* node, const aiScene* scene) {
        // Process all the node's meshes (if any)
        for(GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            mMeshes.push_back(processMesh(mesh, scene));
        }
        // Then do the same for each of its children
        for(GLuint i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    AssimpMesh AssimpModelInstance::processMesh(aiMesh* mesh, const aiScene* scene) {
        vector<Vertex>  vertices;
        vector<GLuint>  indices;
        vector<Texture> textures;

        for(GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // Process vertex positions, normals and texture coordinates
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->mNormals) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for(GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process material
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        return AssimpMesh(vertices, indices, textures);
    }

    vector<Texture> AssimpModelInstance::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            Texture tex = TextureCache::loadTextureFromFile(str.C_Str(), mDirectory.c_str(),typeName.c_str());
            textures.push_back(tex);
        }
        return textures;
    }

    bool AssimpModelInstance::checkGLError(int errorIndex) {
        GLenum errorCode = 0;
        bool wasError = false;
        do {
            errorCode = glGetError();
            if (errorCode!=0) {
                cerr << "AssimpModelInstance: Error Check " << errorIndex << ": " << endl;
                switch (errorCode) {
                    case GL_NO_ERROR:
                        cerr << "\tGL_NO_ERROR" << endl;
                        break;
                    case GL_INVALID_ENUM:
                        cerr << "\tGL_INVALID_ENUM" << endl;
                        break;
                    case GL_INVALID_VALUE:
                        cerr << "\tGL_INVALID_VALUE" << endl;
                        break;
                    case GL_INVALID_OPERATION:
                        cerr << "\tGL_INVALID_OPERATION" << endl;
                        break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION:
                        cerr << "\tGL_INVALID_FRAMEBUFFER_OPERATION" << endl;
                        break;
                    case GL_OUT_OF_MEMORY:
                        cerr << "\tGL_OUT_OF_MEMORY" << endl;
                        break;
                }
                cerr << "\tName: " << glewGetErrorString(errorCode) << endl;
                cerr << "\tCode: " << errorCode << endl;
                wasError = true;
            }
        } while(errorCode != 0);
        return wasError;
    }

    void AssimpModelInstance::loadExtraAttributes(nlohmann::json jsonData) {}

} // End of Dream
