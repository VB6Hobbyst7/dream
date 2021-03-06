/*
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 */


#pragma once



#include <iostream>
#include <map>
#include <vector>

#ifdef DREAM_LOG
    #include <spdlog/spdlog.h>
    #include <spdlog/sinks/stdout_sinks.h>
#endif

using namespace std;

namespace Dream
{
    /**
     * @brief AssetType enum describing the types of Asset available in Dream.
     */
    enum AssetType
    {
        ANIMATION,
        AUDIO,
        FONT,
        LIGHT,
        MATERIAL,
        MODEL,
        OBJECT_EMITTER,
        PARTICLE_EMITTER,
        PATH,
        PHYSICS_OBJECT,
        SCRIPT,
        SCROLLER,
        SHADER,
        TEXTURE,
        NONE
    };

    /**
     * @brief Constants that are used throughout Dream.
     */
    class Constants
    {
    public:

    #ifdef DREAM_LOG
        // Debug ================================================================
        /**
         * @brief Used to check for OpenGL Runtime Errors. This will display the
         * file and line from which the error was detected. This function should
         * not be called directly. Instead use the macro checkGLError.
         *
         * @return True if an error was detected.
         */
        static bool checkGLError_(const string &, int);

        #define checkGLError() Constants::checkGLError_(__FILE__, __LINE__)
    #endif

        // Misc =================================================================
        const static string DIR_PATH_SEP;
        const static string DIR_PATH_SEP_WINDOWS;
        const static string PATH_DELIMETER;
        const static string HTTP_ARG;
        // Project ==============================================================
        const static string PROJECT_DEFAULT_NAME;
        const static string PROJECT_DIRECTORY_ARG;
        const static string PROJECT_UUID_ARG;
        const static int    PROJECT_UUID_LENGTH;
        const static string PROJECT_EXTENSION;
        const static string PROJECT_PATH_SEP;
        const static string PROJECT_UUID;
        const static string PROJECT_NAME;
        const static string PROJECT_AUTHOR;
        const static string PROJECT_DESCRIPTION;
        const static string PROJECT_SCENE_ARRAY;
        const static string PROJECT_ASSET_ARRAY;
        const static string PROJECT_STARTUP_SCENE;
        const static string PROJECT_WINDOW_SIZE;
        const static string PROJECT_WINDOW_WIDTH;
        const static string PROJECT_WINDOW_HEIGHT;
        const static int    PROJECT_DEFAULT_WINDOW_WIDTH;
        const static int    PROJECT_DEFAULT_WINDOW_HEIGHT;

        // Asset ================================================================
        const static string UUID;
        const static string NAME;
        const static string ASSET_ATTR_GROUP;
        const static string ASSET_ATTR_GROUP_DEFAULT;
        //const static string ASSET_DIR;

        // Types
        const static string ASSET_TYPE;
        const static string ASSET_TYPE_ANIMATION;
        const static string ASSET_TYPE_AUDIO;
        const static string ASSET_TYPE_LIGHT;
        const static string ASSET_TYPE_FONT;
        const static string ASSET_TYPE_MODEL;
        const static string ASSET_TYPE_OBJECT_EMITTER;
        const static string ASSET_TYPE_PATH;
        const static string ASSET_TYPE_PHYSICS_OBJECT;
        const static string ASSET_TYPE_SCRIPT;
        const static string ASSET_TYPE_SCROLLER;
        const static string ASSET_TYPE_SHADER;
        const static string ASSET_TYPE_TEXTURE;
        const static string ASSET_TYPE_MATERIAL;
        const static string ASSET_TYPE_PARTICLE_EMITTER;

        // Formats
        const static string ASSET_FORMAT;
        const static string ASSET_FORMAT_DREAM;
        const static string ASSET_FORMAT_SCRIPT_ANGELSCRIPT;
        const static string ASSET_FORMAT_MODEL_ASSIMP;
        const static string ASSET_FORMAT_MODEL_OBJ;
        const static string ASSET_FORMAT_AUDIO_OGG;
        const static string ASSET_FORMAT_AUDIO_WAV;
        const static string ASSET_FORMAT_FONT_TTF;
        const static string ASSET_FORMAT_SHADER_GLSL;
        const static string ASSET_FORMAT_LIGHT_POINT;
        const static string ASSET_FORMAT_LIGHT_DIRECTIONAL;
        const static string ASSET_FORMAT_LIGHT_SPOTLIGHT;
        const static string ASSET_DEFINITION_DEFAULT_NAME;

        // Readable Names
        const static string ASSET_TYPE_ANIMATION_READABLE;
        const static string ASSET_TYPE_PATH_READABLE;
        const static string ASSET_TYPE_AUDIO_READABLE;
        const static string ASSET_TYPE_LIGHT_READABLE;
        const static string ASSET_TYPE_FONT_READABLE;
        const static string ASSET_TYPE_MODEL_READABLE;
        const static string ASSET_TYPE_OBJECT_EMITTER_READABLE;
        const static string ASSET_TYPE_PHYSICS_OBJECT_READABLE;
        const static string ASSET_TYPE_SCRIPT_READABLE;
        const static string ASSET_TYPE_SCROLLER_READABLE;
        const static string ASSET_TYPE_SHADER_READABLE;
        const static string ASSET_TYPE_TEXTURE_READABLE;
        const static string ASSET_TYPE_MATERIAL_READABLE;
        const static string ASSET_TYPE_PARTICLE_EMITTER_READABLE;


        const static string ASSET_FORMAT_READABLE;
        const static string ASSET_FORMAT_SCRIPT_ANGELSCRIPT_READABLE;
        const static string ASSET_FORMAT_MODEL_ASSIMP_READABLE;
        const static string ASSET_FORMAT_MODEL_OBJ_READABLE;
        const static string ASSET_FORMAT_AUDIO_OGG_READABLE;
        const static string ASSET_FORMAT_AUDIO_WAV_READABLE;
        const static string ASSET_FORMAT_DREAM_READABLE;
        const static string ASSET_FORMAT_FONT_TTF_READABLE;
        const static string ASSET_FORMAT_SHADER_GLSL_READABLE;
        const static string ASSET_FORMAT_LIGHT_POINT_READABLE;
        const static string ASSET_FORMAT_LIGHT_DIRECTIONAL_READABLE;
        const static string ASSET_FORMAT_LIGHT_SPOTLIGHT_READABLE;

        // Scene ================================================================
        const static string SCENE_NOTES;
        const static string SCENE_ROOT_ACTOR;
        const static string SCENE_CAMERA_TRANSFORM;
        const static string SCENE_CAMERA_MOVEMENT_SPEED;
        const static float  SCENE_CAMERA_DEFAULT_MOVEMENT_SPEED;
        const static string SCENE_MOVEMENT_SPEED;
        const static string SCENE_CLEAR_COLOUR;
        const static string SCENE_AMBIENT_LIGHT_COLOUR;
        const static string SCENE_GRAVITY;
        const static string SCENE_PHYSICS_DEBUG;
        const static string SCENE_DEFAULT_NAME;
        const static string SCENE_MESH_CULL_DISTANCE;
        const static string SCENE_MIN_DRAW_DISTANCE;
        const static string SCENE_MAX_DRAW_DISTANCE;
        const static string SCENE_CAMERA_TRANSLATION;
        const static string SCENE_CAMERA_LOOK_AT;
        const static string SCENE_CAMERA_PITCH;
        const static string SCENE_CAMERA_YAW;
        const static string SCENE_CAMERA_FOCUSED_ON;
        const static string SCENE_LIGHTING_PASS_SHADER;
        const static string SCENE_SHADOW_PASS_SHADER;
        const static string SCENE_INPUT_SCRIPT;


        // Actor ==========================================================
        const static string ACTOR;
        const static string ACTOR_ROOT_NAME;
        const static string ACTOR_DEFAULT_NAME;
        const static string ACTOR_CHILDREN;
        const static string ACTOR_HAS_CAMERA_FOCUS;
        const static string ACTOR_ASSET_INSTANCES;
        const static string ACTOR_ALWAYS_DRAW;
        const static string ACTOR_TEMPLATE;
        const static string ACTOR_HIDDEN;
        const static string ACTOR_DEFERRED;
        const static string ACTOR_DIE_AFTER;
        const static string ACTOR_PLAYER_OBJECT;

        // Transform ============================================================
        const static string TRANSFORM;
        const static string TRANSFORM_MATRIX;

        // Generic ==============================================================
        const static string X;
        const static string Y;
        const static string Z;
        const static string W;
        const static unsigned int X_INDEX;
        const static unsigned int Y_INDEX;
        const static unsigned int Z_INDEX;
        const static unsigned int W_INDEX;
        const static string RED;
        const static string GREEN;
        const static string BLUE;
        const static string ALPHA;
        const static unsigned int RED_INDEX;
        const static unsigned int GREEN_INDEX;
        const static unsigned int BLUE_INDEX;
        const static unsigned int ALPHA_INDEX;
        const static unsigned int XYZ_VECTOR_SIZE;
        const static unsigned int RGB_VECTOR_SIZE;
        const static unsigned int RGBA_VECTOR_SIZE;

        // Path =============================================================
        const static string ASSET_ATTR_ID;
        const static string ASSET_ATTR_VELOCITY;
        const static string ASSET_ATTR_POSITION;
        const static string ASSET_ATTR_KEYFRAMES;
        const static string ASSET_ATTR_START_TIME;
        const static string ASSET_ATTR_WRAP;
        const static string ASSET_ATTR_LOOP;
        const static string ASSET_ATTR_INTERPOLATION;
        const static string ASSET_ATTR_CONTROL_POINTS;
        const static string ASSET_ATTR_INDEX;
        const static int DREAM_PATH_STOPPED;
        const static int DREAM_PATH_PLAYING;
        const static int DREAM_PATH_PAUSED;
        const static int MAX_FRAME_ADVANCE;
        const static string DREAM_PATH_SPLINE_TYPE;
        const static string DREAM_PATH_TYPE_OPEN;
        const static string DREAM_PATH_TYPE_CLAMPED;
        const static string DREAM_PATH_TYPE_BEZIER;
        const static string ASSET_ATTR_STEP_SCALAR;

        // Particle Emitter ========================================================
        const static string PARTICLE_EMITTER_TEXTURE;
        const static string PARTICLE_EMITTER_AREA;
        const static string PARTICLE_EMITTER_SIZE;
        const static string PARTICLE_EMITTER_VELOCITY;
        const static string PARTICLE_EMITTER_GRAVITY;
        const static string PARTICLE_EMITTER_PER_SECOND;
        const static string PARTICLE_EMITTER_LIFETIME;

        // Object Emitter ======================================================
        const static string OBJECT_EMITTER_UUID;
        const static string OBJECT_EMITTER_OBJECT_COUNT;
        const static string OBJECT_EMITTER_EMIT_INTERVAL;
        const static string OBJECT_EMITTER_LOOP_INTERVAL;
        const static string OBJECT_EMITTER_LOOPS;
        const static string OBJECT_EMITTER_VELOCITY;
        const static string OBJECT_EMITTER_START_THETA;
        const static string OBJECT_EMITTER_END_THETA;
        const static string OBJECT_EMITTER_START_RADIUS;

        // Physics ==============================================================
        const static string ASSET_ATTR_MASS;
        const static string ASSET_ATTR_MARGIN;
        const static string ASSET_ATTR_RADIUS;
        const static string ASSET_ATTR_HEIGHT;
        const static string ASSET_ATTR_NORMAL;
        const static string ASSET_ATTR_CONSTANT;
        const static string ASSET_ATTR_SIZE;
        const static string ASSET_ATTR_COMPOUND_CHILDREN;
        const static string ASSET_ATTR_COLLISION_MODEL;
        const static string ASSET_ATTR_KINEMATIC;
        const static string ASSET_ATTR_CONTROLLABLE;
        const static string ASSET_ATTR_RESTITUTION;
        const static string ASSET_ATTR_FRICTION;
        const static string ASSET_ATTR_CCD_SPR;
        const static string ASSET_ATTR_ANGULAR_FACTOR;
        const static string ASSET_ATTR_LINEAR_FACTOR;
        const static string ASSET_ATTR_ANGULAR_VELOCITY;
        const static string ASSET_ATTR_LINEAR_VELOCITY;


        const static string COLLISION_SHAPE_SPHERE;
        const static string COLLISION_SHAPE_BOX;
        const static string COLLISION_SHAPE_CYLINDER;
        const static string COLLISION_SHAPE_CAPSULE;
        const static string COLLISION_SHAPE_CONE;
        const static string COLLISION_SHAPE_MULTI_SPHERE;
        const static string COLLISION_SHAPE_CONVEX_HULL;
        const static string COLLISION_SHAPE_CONVEX_TRIANGLE_MESH;
        const static string COLLISION_SHAPE_BVH_TRIANGLE_MESH;
        const static string COLLISION_SHAPE_HEIGHTFIELD_TERRAIN;
        const static string COLLISION_SHAPE_STATIC_PLANE;
        const static string COLLISION_SHAPE_COMPOUND;

        const static string COLLISION_SHAPE_SPHERE_READABLE;
        const static string COLLISION_SHAPE_BOX_READABLE;
        const static string COLLISION_SHAPE_CYLINDER_READABLE;
        const static string COLLISION_SHAPE_CAPSULE_READABLE;
        const static string COLLISION_SHAPE_CONE_READABLE;
        const static string COLLISION_SHAPE_MULTI_SPHERE_READABLE;
        const static string COLLISION_SHAPE_CONVEX_HULL_READABLE;
        const static string COLLISION_SHAPE_CONVEX_TRIANGLE_MESH_READABLE;
        const static string COLLISION_SHAPE_BVH_TRIANGLE_MESH_READABLE;
        const static string COLLISION_SHAPE_HEIGHTFIELD_TERRAIN_READABLE;
        const static string COLLISION_SHAPE_STATIC_PLANE_READABLE;
        const static string COLLISION_SHAPE_COMPOUND_READABLE;

        // Event ================================================================
        const static string EVENT_SENDER;
        const static string EVENT_TYPE;
        const static string EVENT_TYPE_COLLISION;

        // Light ================================================================
        const static string ASSET_ATTR_LIGHT_COLOR;
        const static string ASSET_ATTR_LIGHT_AMBIENT;
        const static string ASSET_ATTR_LIGHT_DIFFUSE;
        const static string ASSET_ATTR_LIGHT_SPECULAR;
        const static string ASSET_ATTR_LIGHT_DIRECTION;
        const static string ASSET_ATTR_LIGHT_CONSTANT;
        const static string ASSET_ATTR_LIGHT_LINEAR;
        const static string ASSET_ATTR_LIGHT_QUADRATIC;
        const static string ASSET_ATTR_LIGHT_CUTOFF;
        const static string ASSET_ATTR_LIGHT_OUTER_CUTOFF;

        // Material =============================================================
        const static string ASSET_ATTR_MATERIAL_SHADER;
        const static string ASSET_ATTR_MATERIAL_DIFFUSE_TEXTURE;
        const static string ASSET_ATTR_MATERIAL_SPECULAR_TEXTURE;
        const static string ASSET_ATTR_MATERIAL_NORMAL_TEXTURE;
        const static string ASSET_ATTR_MATERIAL_DISPLACEMENT_TEXTURE;
        const static string ASSET_ATTR_MATERIAL_DIFFUSE_COLOUR;
        const static string ASSET_ATTR_MATERIAL_SPECULAR_COLOUR;
        const static string ASSET_ATTR_MATERIAL_AMBIENT_COLOUR;
        const static string ASSET_ATTR_MATERIAL_REFLECTIVE_COLOUR;
        const static string ASSET_ATTR_MATERIAL_EMISSIVE_COLOUR;

        const static string ASSET_ATTR_MATERIAL_OPACITY;
        const static string ASSET_ATTR_MATERIAL_BUMP_SCALING;
        const static string ASSET_ATTR_MATERIAL_HARDNESS;
        const static string ASSET_ATTR_MATERIAL_REFLECTIVITY;
        const static string ASSET_ATTR_MATERIAL_SHININESS_STRENGTH;
        const static string ASSET_ATTR_MATERIAL_REFRACTION_INDEX;
        const static string ASSET_ATTR_MATERIAL_IGNORE;

        // Model ===============================================================
        const static string ASSET_ATTR_MODEL_MATERIAL_LIST;
        const static string ASSET_ATTR_MODEL_MODEL_MATERIAL;
        const static string ASSET_ATTR_MODEL_DREAM_MATERIAL;

        // Shader ==============================================================
        const static string SHADER_FRAGMENT;
        const static string SHADER_VERTEX;
        const static string SHADER_FRAGMENT_FILE_NAME;
        const static string SHADER_VERTEX_FILE_NAME;

        // Script ==============================================================
        const static string SCRIPT_INIT_FUNCTION;
        const static string SCRIPT_UPDATE_FUNCTION;
        const static string SCRIPT_INPUT_FUNCTION;
        const static string SCRIPT_EVENT_FUNCTION;
        const static string SCRIPT_DESTROY_FUNCTION;

        // Scroller ============================================================
        const static string SCROLLER_VELOCITY;
        const static string SCROLLER_LOOP;
        const static string SCROLLER_RANGE_BEGIN;
        const static string SCROLLER_RANGE_END;
        const static string SCROLLER_ITEMS_ARRAY;
        const static string SCROLLER_ITEM_INDEX;
        const static string SCROLLER_ITEM_ORIGIN;

        // Audio ================================================================
        const static string ASSET_ATTR_SPECTRUM_ANALYSER;
        const static string ASSET_ATTR_AUDIO_EVENT_MARKERS;
        const static string ASSET_ATTR_AUDIO_EM_INDEX;
        const static string ASSET_ATTR_AUDIO_EM_S_INDEX;
        const static string ASSET_ATTR_AUDIO_EM_NAME;
        const static string ASSET_ATTR_AUDIO_EM_REPEAT;
        const static string ASSET_ATTR_AUDIO_EM_REPEAT_PERIOD;

        // Animation/Keyframe ==================================================
        const static string KEYFRAME_ID;
        const static string KEYFRAME_TIME;
        const static string KEYFRAME_TRANSLATION;
        const static string KEYFRAME_ROTATION;
        const static string KEYFRAME_SCALE;
        const static string KEYFRAME_DURATION;
        const static string KEYFRAME_LOOPING;
        const static string KEYFRAME_EASING_TYPE;
        const static string ANIMATION_RELATIVE;

        // Window ==============================================================
        const static int DEFAULT_SCREEN_WIDTH;
        const static int DEFAULT_SCREEN_HEIGHT;

        // Camera ==============================================================
        const static unsigned int CAMERA_MOVEMENT_FORWARD;
        const static unsigned int CAMERA_MOVEMENT_BACKWARD;
        const static unsigned int CAMERA_MOVEMENT_LEFT;
        const static unsigned int CAMERA_MOVEMENT_RIGHT;
        const static unsigned int CAMERA_MOVEMENT_UP;
        const static unsigned int CAMERA_MOVEMENT_DOWN;
        const static float CAMERA_PITCH_MAX;
        const static float CAMERA_ZOOM_MIN;
        const static float CAMERA_ZOOM_MAX;
        const static float CAMERA_YAW;
        const static float CAMERA_YAW_OFFSET;
        const static float CAMERA_PITCH;
        const static float CAMERA_SPEED;
        const static float CAMERA_SENSITIVTY;
        const static float CAMERA_ZOOM;

        // Data Maps ===========================================================
        static map<AssetType,string> DREAM_ASSET_TYPES_MAP;
        static vector<string> DREAM_ASSET_TYPES_READABLE_VECTOR;
        static map<AssetType,vector<string> > DREAM_ASSET_FORMATS_MAP;
        static vector<string> DREAM_PATH_SPLINE_TYPES;

        static string getAssetTypeStringFromTypeEnum(AssetType type);
        static AssetType getAssetTypeEnumFromString(const string &type);

        static string getAssetTypeReadableNameFromString(const string&);

        static string getAssetFormatStringFromReadableName(const string& format);
        static string getAssetFormatReadableNameFromString(const string&);

        #ifdef DREAM_LOG
        static shared_ptr<spdlog::logger> logger;
        #endif
    };
}

