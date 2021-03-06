#include "Cursor.h"
#include "../../DTState.h"
#include "../../../DreamCore/Common/Constants.h"
#include "../../../DreamCore/Project/Project.h"
#include "../../../DreamCore/Project/ProjectRuntime.h"
#include "../../../DreamCore/Components/Graphics/GraphicsComponent.h"
#include "../../../DreamCore/Components/Graphics/Shader/ShaderRuntime.h"
#include "../../../DreamCore/Scene/SceneRuntime.h"

namespace DreamTool
{
    Cursor::Cursor(DTState* state)
        : GLWidget(state,false),
          mStepMajor(false),
          mOrientation(mat4(1.0f))
    {
#ifdef DREAM_LOG
        setLogClassName("Cursor");
        getLog()->trace("Constructing");
#endif
        for (auto index : ModelIndices)
        {
            mVertexBuffer.push_back(ModelVertices.at(index));
        }
    }

    Cursor::~Cursor()
    {
#ifdef DREAM_LOG
        getLog()->trace("Destructing");
#endif
    }

    void
    Cursor::draw
    ()
    {
#ifdef DREAM_LOG
        auto log = getLog();
#endif

        if (mState->project)
        {
            auto pRuntime = mState->project->getRuntime();
            if (pRuntime)
            {
                auto sRunt = pRuntime->getActiveSceneRuntime();
                if (sRunt)
                {
                    auto cam = sRunt->getCamera();
                    if (cam)
                    {
                        mProjectionMatrix = cam->getProjectionMatrix();
                        mViewMatrix = cam->getViewMatrix();
                    }
                }
            }
        }

        if (!mVertexBuffer.empty())
        {
#ifndef __APPLE__
            glEnable(GL_LINE_SMOOTH);
            #ifdef DREAM_LOG
            checkGLError();
            #endif
            glLineWidth(3.0f);
            #ifdef DREAM_LOG
            checkGLError();
            #endif
            #endif
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            // Enable shader program
            glUseProgram(mShaderProgram);
            ShaderRuntime::CurrentShaderProgram = mShaderProgram;
#ifdef DREAM_LOG
            checkGLError();
#endif


            // Vertex Array
            glBindVertexArray(mVao);
            ShaderRuntime::CurrentVAO = mVao;
#ifdef DREAM_LOG
            checkGLError();
#endif

            glBindBuffer(GL_ARRAY_BUFFER, mVbo);
            ShaderRuntime::CurrentVBO = mVbo;
#ifdef DREAM_LOG
            checkGLError();
#endif

#ifdef DREAM_LOG
            checkGLError();
#endif
            if (mProjectionUniform == -1)
            {
#ifdef DREAM_LOG
                log->error("Unable to find Uniform Location for projection");
#endif
                return;
            }
            else
            {
                glUniformMatrix4fv(mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
#ifdef DREAM_LOG
                checkGLError();
#endif
            }

            // Set the view matrix
#ifdef DREAM_LOG
            checkGLError();
#endif
            if (mViewUniform == -1)
            {
#ifdef DREAM_LOG
                log->error("Unable to find Uniform Location for view");
#endif
                return;
            }
            else
            {
                glUniformMatrix4fv(mViewUniform, 1, GL_FALSE, glm::value_ptr(mViewMatrix));
#ifdef DREAM_LOG
                checkGLError();
#endif
            }

            // Set the projection matrix
            if (mModelUniform == -1)
            {
#ifdef DREAM_LOG
                log->error("Unable to find Uniform Location for model");
#endif
                return;
            }
            else
            {
                glUniformMatrix4fv(mModelUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
#ifdef DREAM_LOG
                checkGLError();
#endif
            }

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertexBuffer.size()));
#ifdef DREAM_LOG
            checkGLError();
#endif

            // Revert State
#ifndef __APPLE__
            glDisable(GL_LINE_SMOOTH);
            glLineWidth(1.0f);
    #ifdef DREAM_LOG
                checkGLError();
    #endif
#endif
            glDisable(GL_BLEND);
        }
    }

    void Cursor::init()
    {
        GLWidget::init();
        // Vertex Array
        glBindVertexArray(mVao);
        ShaderRuntime::CurrentVAO = mVao;
#ifdef DREAM_LOG
        checkGLError();
#endif
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        ShaderRuntime::CurrentVBO = mVbo;
#ifdef DREAM_LOG
        checkGLError();
#endif
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(mVertexBuffer.size() * sizeof(GLWidgetVertex)), &mVertexBuffer[0], GL_STATIC_DRAW);
#ifdef DREAM_LOG
        checkGLError();
#endif
        glBindVertexArray(0);
#ifdef DREAM_LOG
        checkGLError();
#endif
    }

    void
    Cursor::onAction
    (CursorAction a)
    {
        vec3 txDelta(0.0f);
        float step = mStepMajor ?
                     mState->grid.getMajorSpacing() :
                     mState->grid.getMinorSpacing();

        // Make a move
        switch(a)
        {
            case DreamTool::Cursor::XPlus:
                txDelta.x = step;
                break;
            case DreamTool::Cursor::XMinus:
                txDelta.x = -step;
                break;
            case DreamTool::Cursor::YPlus:
                txDelta.y = step;
                break;
            case DreamTool::Cursor::YMinus:
                txDelta.y = -step;
                break;
            case DreamTool::Cursor::ZPlus:
                txDelta.z = step;
                break;
            case DreamTool::Cursor::ZMinus:
                txDelta.z = -step;
                break;
            case DreamTool::Cursor::StepMajor:
                break;
            case DreamTool::Cursor::StepMinor:
                break;
        }

        // Cancel unused axis
        switch (mState->grid.getAxisPair())
        {
            case DreamTool::Grid::XY:
                txDelta.z = 0.0f;
                break;
            case DreamTool::Grid::XZ:
                txDelta.y = 0.0f;
                break;
            case DreamTool::Grid::YZ:
                txDelta.x = 0.0f;
                break;
        }

        vec3 currentTx = vec3(mModelMatrix[3]);
        mModelMatrix = glm::translate(mat4(1.0f),currentTx+txDelta) * mOrientation;
    }

    void
    Cursor::setPosition
    (vec3 pos, bool snap)
    {
        if (snap)
        {
            float minor = mState->grid.getMinorSpacing();
            pos.x = pos.x-fmod(pos.x,minor);
            pos.y = pos.y-fmod(pos.y,minor);
            pos.z = pos.z-fmod(pos.z,minor);
        }

        switch (mState->grid.getAxisPair())
        {
            case DreamTool::Grid::XY:
                pos.z = 0.0f;
                break;
            case DreamTool::Grid::XZ:
                pos.y = 0.0f;
                break;
            case DreamTool::Grid::YZ:
                pos.x = 0.0f;
                break;
        }
        mModelMatrix = translate(mat4(1.0f),pos) * mOrientation;
    }

    void
    Cursor::onAxisPairChanged
    (Grid::AxisPair ap)
    {
        vec3 current = getPosition();
        switch (ap)
        {
            case DreamTool::Grid::XY:
                current.z = 0.0f;
                mOrientation = mat4_cast(quat(vec3(radians(90.0f),0.0f,0.0f)));
                break;
            case DreamTool::Grid::XZ:
                current.y = 0.0f;
                mOrientation = mat4_cast(quat(vec3(0.0f,0.0f,0.0f)));
                break;
            case DreamTool::Grid::YZ:
                current.x = 0.0f;
                mOrientation = mat4_cast(quat(vec3(0.0f,0.0f,radians(-90.0f))));
                break;
        }
        mModelMatrix = glm::translate(mat4(1.0f),current) * mOrientation;
    }

    void
    Cursor::setMousePosition
    (float x, float y)
    {
        vec3 pos = mouseToWorldSpace(x,y);
        setPosition(pos,false);
    }

    vec3
    Cursor::mouseToWorldSpace
    (float x, float y)
    {
        auto w =  mState->windowComponent.getWidth();
        auto h = mState->windowComponent.getHeight();
        float depth = 0.5;
        /*if (mState->project->getRuntime())
        {
            auto camTx = mState->project->getRuntime()->getActiveSceneRuntime()->getCamera()->getTranslation();
            depth = glm::distance(vec3(0.0f),camTx);
        }
        */
        mat4 mtx(1.0f);
        vec3 out = glm::unProject(
            vec3(x, y, depth),
            mtx*mViewMatrix,
            mProjectionMatrix,
            vec4(0.0f,0.0f,w,h)
        );
        return out;
    }

    const vector<GLWidgetVertex> Cursor::ModelVertices =
    {
        GLWidgetVertex{vec3(0.000f, 0.000f, 0.000f), vec3(0.0f,1.0f,0.0f)},
        GLWidgetVertex{vec3(-1.000f, 4.000f, 1.000f), vec3(0.0f,1.0f,0.0f)},
        GLWidgetVertex{vec3(-1.000f, 4.000f, -1.000f), vec3(0.0f,1.0f,0.0f)},
        GLWidgetVertex{vec3(1.000f, 4.000f, 1.000f), vec3(0.0f,1.0f,0.0f)},
        GLWidgetVertex{vec3(1.000f, 4.000f, -1.000f), vec3(0.0f,1.0f,0.0f)},
    };

    const vector<GLuint> Cursor::ModelIndices =
    {
        0, 1, 2,
        0, 2, 4,
        0, 4, 3,
        0, 3, 1,
        3, 4, 1,
        1, 4, 2,
    };
}
