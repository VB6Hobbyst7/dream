#include "AssimpMesh.h"
#include "AssimpModelInstance.h"

namespace Dream
{

    AssimpMesh::AssimpMesh
    (
        AssimpModelInstance* parent,
        string name,
        vector<Vertex> vertices,
        vector<GLuint> indices,
        AssimpMaterial* material
    ) : ILoggable("AssimpMesh"),
        mParentHandle(parent),
        mMaterialHandle(material),
        mName(name),
        mVertices(vertices),
        mIndices(indices)
    {
        auto log = getLog();
        log->trace("Constructing Mesh for {}", parent->getName());
        init();
    }

    AssimpMesh::~AssimpMesh
    ()
    {
        auto log = getLog();
        log->trace("Destroying Mesh for {}",mParentHandle->getNameAndUuidString());
    }

    string AssimpMesh::getName() const
    {
        return mName;
    }

    void AssimpMesh::setName(const string& name)
    {
        mName = name;
    }

    BoundingBox AssimpMesh::getBoundingBox() const
    {
        return mBoundingBox;
    }

    void AssimpMesh::setBoundingBox(const BoundingBox& boundingBox)
    {
        mBoundingBox = boundingBox;
    }

    vector<Vertex> AssimpMesh::getVertices() const
    {
        return mVertices;
    }

    vector<GLuint> AssimpMesh::getIndices() const
    {
        return mIndices;
    }

    void
    AssimpMesh::bindTextures
    (ShaderInstance*)
    {
        auto log = getLog();
        bindTexture(mMaterialHandle->mDiffuseTexture.get());
        bindTexture(mMaterialHandle->mSpecularTexture.get());
        bindTexture(mMaterialHandle->mNormalTexture.get());
        glActiveTexture(GL_TEXTURE0);
    }

    void AssimpMesh::bindTexture(Texture* t)
    {
        if (t == nullptr)
        {
            return;
        }
        auto log = getLog();
        string name = t->type;
        GLuint nextTexture = 0;

        if (t->id == 0)
        {
            return;
        }

        if(name == "texture_diffuse")
        {
            // Activate proper texture unit before binding
            log->critical("Binding diffuse texture");
            nextTexture = GL_TEXTURE0;
        }
        else if(name == "texture_specular")
        {
            // Activate proper texture unit before binding
            log->critical("Binding specular texture");
            nextTexture = GL_TEXTURE1;
        }
        else if (name == "texture_normals")
        {
            // Activate proper texture unit before binding
            log->critical("Binding normals texture");
            nextTexture = GL_TEXTURE2;
        }

        log->info(
              "Binding Material {} with GL Texture {} to unit {} for {} in {}",
              name,
              t->id,
              nextTexture,
              getName(),
              mParentHandle->getNameAndUuidString()
         );

        glActiveTexture(nextTexture);
        glBindTexture(GL_TEXTURE_2D, t->id);
    }

    void
    AssimpMesh::unbindTextures
    ()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,0);

    }

    void
    AssimpMesh::bindDiffuse
    (ShaderInstance *shaderHandle)
    {
        auto log = getLog();
        aiColor4D diff = mMaterialHandle->mColorDiffuse;
        auto diffuse = vec4(diff.r, diff.g, diff.b, diff.a);
        log->trace("Material Diffuse for {}: ({},{},{},{})",getName(),diff.r, diff.g, diff.b, diff.a);
        shaderHandle->addUniform(FLOAT4,"materialDiffuseColour",1,&diffuse);
    }

    void
    AssimpMesh::bindSpecular
    (ShaderInstance *shaderHandle)
    {
        auto log = getLog();
        aiColor4D spec = mMaterialHandle->mColorSpecular;
        auto specular = vec4(spec.r, spec.g, spec.b, spec.a);
        log->trace(
            "Material Specular for {}: ({},{},{},{}) strength {}",
            getName(),
            spec.r, spec.g, spec.b, spec.a,
            mMaterialHandle->mShininessStrength
        );
        shaderHandle->addUniform(FLOAT4,"materialSpecularColour",1,&specular);
        shaderHandle->addUniform(FLOAT1,"materialShininess",1,&mMaterialHandle->mShininessStrength);
    }

    void
    AssimpMesh::bindAmbient
    (ShaderInstance *shaderHandle)
    {
        auto log = getLog();
        aiColor4D amb = mMaterialHandle->mColorAmbient;
        log->trace(
            "Material Ambient for {}: ({},{},{},{})",
            getName(),
            amb.r, amb.g, amb.b, amb.a
        );
        auto ambient = vec4(amb.r, amb.g, amb.b, amb.a);
        shaderHandle->addUniform(FLOAT4,"materialAmbientColour",1,&ambient);
    }

    void
    AssimpMesh::bindOpacity
    (ShaderInstance* shaderHandle)
    {
        shaderHandle->addUniform(FLOAT1,"materialOpacity",1,&mMaterialHandle->mOpacity);
    }

    void
    AssimpMesh::draw
    (ShaderInstance* shader)
    {
        auto log = getLog();
        // Bind Shader Values
        //bindTextures(shader);
        //bindDiffuse(shader);
        //bindSpecular(shader);
        //bindOpacity(shader);
        shader->bindMaterial(mMaterialHandle);

        // Sync Uniforms
        shader->syncUniforms();
        checkGLError();

        // Draw mesh
        shader->bindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLint>(mIndices.size()), GL_UNSIGNED_INT, nullptr);
        log->info("Completed a mesh draw");
        //shader->unbindVertexArray();
    }

    void
    AssimpMesh::init
    ()
    {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mIBO);
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(mVertices.size() * sizeof(Vertex)), &mVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(mIndices.size() * sizeof(GLuint)),&mIndices[0], GL_STATIC_DRAW);
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                    0, 3, GL_FLOAT, GL_FALSE,
                    static_cast<GLint>(sizeof(Vertex)),
                    static_cast<GLvoid*>(nullptr)
                    );
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
                    1, 3, GL_FLOAT, GL_FALSE,
                    static_cast<GLint>(sizeof(Vertex)),
                    (GLvoid*)offsetof(Vertex, Normal)
                    );
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
                    2, 2, GL_FLOAT, GL_FALSE,
                    static_cast<GLint>(sizeof(Vertex)),
                    (GLvoid*)offsetof(Vertex, TexCoords)
                    );
        glBindVertexArray(0);
    }
} // End of Dream
