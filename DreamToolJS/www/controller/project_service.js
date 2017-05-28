App.service('ProjectService',
    ["Blob","FileSaver","UtilService","ApiService",
     function(Blob, FileSaver, UtilService, ApiService) {

    this.DEFAULT_WINDOW_WIDTH = 1024;
    this.DEFAULT_WINDOW_HEIGHT = 768;

    // Asset Constants ---------------------------------------------------------

    // Animation
    this.ASSET_TYPE_NAME_ANIMATION    = "Animation";
    this.ASSET_TYPE_ANIMATION         = "animation";
    this.ASSET_FORMAT_NAME_DREAM      = "Dream Animation";
    this.ASSET_FORMAT_ANIMATION_DREAM = "dream";

    this.DREAM_ANIMATION_INTERPOLATION_NONE_NAME = "None";
    this.DREAM_ANIMATION_INTERPOLATION_NONE_TYPE = "none";
    this.DREAM_ANIMATION_INTERPOLATION_LINEAR_NAME = "Linear";
    this.DREAM_ANIMATION_INTERPOLATION_LINEAR_TYPE = "linear";
    this.DREAM_ANIMATION_INTERPOLATION_BEZIER_NAME = "Bezier";
    this.DREAM_ANIMATION_INTERPOLATION_BEZIER_TYPE = "bezier";

    this.DREAM_ANIMATION_INTERPOLATION_TYPES = [
      {
        name: this.DREAM_ANIMATION_INTERPOLATION_NONE_NAME,
        type: this.DREAM_ANIMATION_INTERPOLATION_NONE_TYPE,
      },{
        name: this.DREAM_ANIMATION_INTERPOLATION_LINEAR_NAME,
        type: this.DREAM_ANIMATION_INTERPOLATION_LINEAR_TYPE,
      },{
        name: this.DREAM_ANIMATION_INTERPOLATION_BEZIER_NAME,
        type: this.DREAM_ANIMATION_INTERPOLATION_BEZIER_TYPE,
      }
    ];

    // Audio
    this.ASSET_TYPE_NAME_AUDIO = "Audio";
    this.ASSET_TYPE_AUDIO = "audio";
    this.ASSET_FORMAT_NAME_MUSIC  = "Music";
    this.ASSET_FORMAT_AUDIO_MUSIC = "music";
    this.ASSET_FORMAT_NAME_SFX  = "Sound Effect";
    this.ASSET_FORMAT_AUDIO_SFX = "sfx";

    // Font
    this.ASSET_TYPE_NAME_FONT = "Font";
    this.ASSET_TYPE_FONT = "font";
    this.ASSET_FORMAT_NAME_TRUETYPE = "TrueType";
    this.ASSET_FORMAT_FONT_TRUETYPE = "ttf";

    // Light
    this.ASSET_TYPE_LIGHT = "light";
    this.ASSET_TYPE_NAME_LIGHT = "Light";

    // Physics
    this.ASSET_TYPE_NAME_PHYSICS_OBJECT = "Physics Object";
    this.ASSET_TYPE_PHYSICS_OBJECT = "physicsObject";

    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_SPHERE = "Shpere";
    this.ASSET_FORMAT_PHYSICS_OBJECT_SPHERE = "btShpereShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_BOX = "Box";
    this.ASSET_FORMAT_PHYSICS_OBJECT_BOX = "btBoxShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CYLINDER = "Cylinder";
    this.ASSET_FORMAT_PHYSICS_OBJECT_CYLINDER = "btCylinderShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CAPSULE = "Capsule";
    this.ASSET_FORMAT_PHYSICS_OBJECT_CAPSULE = "btCapsuleShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONE = "Cone";
    this.ASSET_FORMAT_PHYSICS_OBJECT_CONE = "btConeShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_MULTI_SPHERE = "Multi Sphere";
    this.ASSET_FORMAT_PHYSICS_OBJECT_MULTI_SPHERE = "btMultiSphereShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONVEX_HULL = "Convex Hull";
    this.ASSET_FORMAT_PHYSICS_OBJECT_CONVEX_HULL = "btConvexHullShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONVEX_TRIANGLE_MESH = "Convex Triangle Mesh";
    this.ASSET_FORMAT_PHYSICS_OBJECT_CONVEX_TRIANGLE_MESH = "btConvexTriangleMeshShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_BVH_TRIANGLE_MESH = "Bvh Triangle Mesh";
    this.ASSET_FORMAT_PHYSICS_OBJECT_BVH_TRIANGLE_MESH = "btBvhTriangleMeshShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_HEIGHTFIELD_TERRAIN = "Heightfield Terrain";
    this.ASSET_FORMAT_PHYSICS_OBJECT_HEIGHTFIELD_TERRAIN = "btHeightfieldTerrainShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_STATIC_PLANE = "Static Plane";
    this.ASSET_FORMAT_PHYSICS_OBJECT_STATIC_PLANE = "btStaticPlaneShape";
    this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_COMPOUND = "Compound";
    this.ASSET_FORMAT_PHYSICS_OBJECT_COMPOUND = "btCompoundShape";

    // Model
    this.ASSET_TYPE_NAME_MODEL     = "Model";
    this.ASSET_TYPE_MODEL          = "model";
    this.ASSET_FORMAT_NAME_ASSIMP  = "Assimp";
    this.ASSET_FORMAT_MODEL_ASSIMP = "assimp";

    // Script
    this.ASSET_TYPE_NAME_SCRIPT   = "Script";
    this.ASSET_TYPE_SCRIPT        = "script";
    this.ASSET_FORMAT_NAME_CHAI   = "ChaiScript";
    this.ASSET_FORMAT_SCRIPT_CHAI = "chai";
    this.ASSET_FORMAT_NAME_LUA    = "Lua";
    this.ASSET_FORMAT_SCRIPT_LUA  = "lua";

    // Shader
    this.ASSET_TYPE_NAME_SHADER       = "Shader";
    this.ASSET_TYPE_SHADER            = "shader";
    this.ASSET_FORMAT_SHADER_VERTEX   = "vertex";
    this.ASSET_FORMAT_SHADER_FRAGMENT = "fragment";

    // Sprite
    this.ASSET_TYPE_NAME_SPRITE = "Sprite";
    this.ASSET_TYPE_SPRITE = "sprite";
    this.ASSET_FORMAT_NAME_SPRITE = "Sprite";
     this.ASSET_FORMAT_SPRITE = "sprite";


    this.ASSET_TYPES = [
      // Animation
      {
        name: this.ASSET_TYPE_NAME_ANIMATION,
        type: this.ASSET_TYPE_ANIMATION,
        formats: [
          {
            name:   this.ASSET_FORMAT_NAME_DREAM,
            format: this.ASSET_FORMAT_ANIMATION_DREAM
          }
        ]
      },
      // Audio
      {
        name: this.ASSET_TYPE_NAME_AUDIO,
        type: this.ASSET_TYPE_AUDIO,
        formats: [
          {
            name:   this.ASSET_FORMAT_NAME_MUSIC,
            format: this.ASSET_FORMAT_AUDIO_MUSIC
          },
          {
            name:   this.ASSET_FORMAT_NAME_SFX,
            format: this.ASSET_FORMAT_AUDIO_SFX
          }
        ]
      },
      // Font
      {
        name: this.ASSET_TYPE_NAME_FONT,
        type: this.ASSET_TYPE_FONT,
        formats: [{
          name: this.ASSET_FORMAT_NAME_TRUETYPE,
          format: this.ASSET_FORMAT_FONT_TRUETYPE
        }]
      },
      // Light
      {
        name:    this.ASSET_TYPE_NAME_LIGHT,
        type:    this.ASSET_TYPE_LIGHT,
        formats: []
      },
      // Physics Object
      {
        name: this.ASSET_TYPE_NAME_PHYSICS_OBJECT,
        type: this.ASSET_TYPE_PHYSICS_OBJECT,
        formats: [
          {
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_SPHERE,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_SPHERE
          },{
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_BOX,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_BOX
          },{
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CYLINDER,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_CYLINDER
          },{
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CAPSULE,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_CAPSULE
          },{
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONE,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_CONE
          },{
            name:   this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_MULTI_SPHERE,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_MULTI_SPHERE
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONVEX_HULL,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_CONVEX_HULL
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_CONVEX_TRIANGLE_MESH,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_CONVEX_TRIANGLE_MESH
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_BVH_TRIANGLE_MESH,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_BVH_TRIANGLE_MESH
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_HEIGHTFIELD_TERRAIN,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_HEIGHTFIELD_TERRAIN
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_STATIC_PLANE,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_STATIC_PLANE
          },{
            name: this.ASSET_FORMAT_NAME_PHYSICS_OBJECT_COMPOUND,
            format: this.ASSET_FORMAT_PHYSICS_OBJECT_COMPOUND
          }
        ]
      },
      // Model
      {
        name: this.ASSET_TYPE_NAME_MODEL,
        type: this.ASSET_TYPE_MODEL,
        formats: [
          {
            name: this.ASSET_FORMAT_NAME_ASSIMP,
            format: this.ASSET_FORMAT_MODEL_ASSIMP
          }
        ]
      },
      // Shader
      {
        name: this.ASSET_TYPE_NAME_SHADER,
        type: this.ASSET_TYPE_SHADER,
        formats: []
      },
      // Script
      {
        name: this.ASSET_TYPE_NAME_SCRIPT,
        type: this.ASSET_TYPE_SCRIPT,
        formats: [
          {
            name:   this.ASSET_FORMAT_NAME_CHAI,
            format: this.ASSET_FORMAT_SCRIPT_CHAI,
          }, {
              name: this.ASSET_FORMAT_NAME_LUA,
              format: this.ASSET_FORMAT_SCRIPT_LUA
          }
        ]
      },
      // Sprite
      {
          name: this.ASSET_TYPE_NAME_SPRITE,
          type: this.ASSET_TYPE_SPRITE,
          formats: [
            {
              name: this.ASSET_FORMAT_NAME_SPRITE,
              format: this.ASSET_FORMAT_SPRITE
            }
          ]
      }
    ];

    // SceneObject -------------------------------------------------------------

    this.SCENE_OBJECT_TRANSFORM_ABSOLUTE_NAME = "Absolute";
    this.SCENE_OBJECT_TRANSFORM_ABSOLUTE_TYPE = "absolute";

    this.SCENE_OBJECT_TRANSFORM_OFFSET_NAME = "Offset";
    this.SCENE_OBJECT_TRANSFORM_OFFSET_TYPE = "offset";

    this.SCENE_OBJECT_TRANSFORM_TYPES = [{
      type: this.SCENE_OBJECT_TRANSFORM_OFFSET_TYPE,
      name: this.SCENE_OBJECT_TRANSFORM_OFFSET_NAME
    },{
      type: this.SCENE_OBJECT_TRANSFORM_ABSOLUTE_TYPE,
      name: this.SCENE_OBJECT_TRANSFORM_ABSOLUTE_NAME
    }];

    // Methods -----------------------------------------------------------------

    this.getSceneObjectTransformTypes = function() {
      return this.SCENE_OBJECT_TRANSFORM_TYPES;
    };

    this.getFormatsByAssetType = function(assetType,callback) {
      var retval = [];
      this.ASSET_TYPES.forEach(function(type) {
        if (type.name == assetType) {
          retval = type.formats;
        }
      });
      callback(retval);
    };

    this.getAssetTypes = function(callback) {
      var retval = [];
      this.ASSET_TYPES.forEach(function(assetType) {
        retval.push(assetType.type);
      });
      callback(retval);
    };

    this.getAssetTypesObject = function(callback) {
      callback(this.ASSET_TYPES);
    };

    this.getAssetTypeNames = function(callback) {
      var retval = [];
      this.ASSET_TYPES.forEach(function(assetType) {
        retval.push(assetType.name);
      });
      callback(retval);
    };

    this.getAnimationAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_ANIMATION) {
          callback(assetType.formats);
        }
      });
    };

    this.getFontAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_FONT) {
          callback(assetType.formats);
        }
      });
    };

    this.getAudioAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_AUDIO) {
          callback(assetType.formats);
        }
      });
    };

    this.getPhysicsObjectAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_PHYSICS_OBJECT) {
          callback(assetType.formats);
        }
      });
    };

    this.getModelAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_MODEL) {
          callback(assetType.formats);
        }
      });
    };

    this.getScriptAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_SCRIPT) {
          callback(assetType.formats);
        }
      });
    };

    this.getSpriteAssetFormats = function(callback) {
      var ps = this;
      this.ASSET_TYPES.forEach(function(assetType){
        if (assetType.type == ps.ASSET_TYPE_SPRITE) {
          callback(assetType.formats);
        }
      });
    };

    // Functions && Variables --------------------------------------------------

    this.project = null;

    this.isProjectOpen = function() {
        return this.project !== null;
    };

    this.getProject = function() {
        return this.project;
    };

    this.getProjectUUID = function() {
        return this.project.uuid;
    };

    this.getName = function() {
        return this.project.name;
    };

    this.setName = function(name) {
      if (this.project) {
        this.project.isModified = true;
        this.project.name = name;
      }
    };

    this.setAuthor = function(author) {
      if (this.project) {
        this.project.isModified = true;
        this.project.author = author;
      }
    };

    this.setDescription = function(description) {
      if (this.project) {
        this.project.isModified = true;
        this.project.description = description;
      }
    };

    this.setStartupScene = function(startupScene) {
      if (this.project) {
        this.project.isModified = true;
        this.project.startupScene = startupScene;
      }
    };

    this.updateProject = function(project) {
        this.setName        (project.name);
        this.setAuthor      (project.author);
        this.setDescription (project.description);
        this.setStartupScene(project.startupScene);
    };

    this.getSceneList = function(callback) {
        var retval = [];
        this.project.scenes.forEach(function(nextScene){
          retval.push({
            uuid: nextScene.uuid,
            name: nextScene.name});
        });
        callback(retval);
    };

    this.pushScene = function(scene) {
        this.project.isModified = true;
        this.project.scenes.push(scene);
    };

    this.getSceneByUUID = function(name, callback) {
        var retval = null;
        this.project.scenes.forEach(function (scene){
            if (scene.uuid === name) {
                retval = scene;
            }
        });
        callback(retval);
    };

    this.removeScene = function(scene) {
        var index = this.project.scenes.indexOf(scene);
        this.project.scenes.splice(index,1);
    };

    this.pushAsset = function(asset) {
        this.project.isModified = true;
        this.project.assets.push(asset);
    };

    this.getAssetByUUID = function(uuid, callback) {
        var retval = null;

        if (this.project === null) {
            callback(retval);
            return;
        }

        this.project.assets.forEach(function (rsc) {
            if (rsc.uuid === uuid) {
                retval = rsc;
            }
        });
        callback(retval);
    };

    this.removeAsset = function(asset) {
        var index = this.project.assets.indexOf(asset);
        this.project.assets.splice(index,1);
    };


    this.addAssetInstanceToSceneObject = function(sceneUUID,sceneObjectUUID,assetUUID,callback){
        var proj = this;
        console.log("Adding Asset Instance",sceneObjectUUID,"to",assetUUID);
        this.getSceneByUUID(sceneUUID,function(scene){
            proj.getSceneObjectByUUID(scene,sceneObjectUUID,function(sceneObject){
                if (sceneObject.assetInstances === undefined) {
                    sceneObject.assetInstances = [];
                }
                if (sceneObject.assetInstances.indexOf(assetUUID) < 0) {
                    sceneObject.assetInstances.push(assetUUID);
                    callback(true);
                } else {
                    console.log(sceneObjectUUID,"allready has instance to",assetUUID);
                    callback(false);
                }
            });
        });
    };

    this.removeAssetInstanceFromSceneObject = function(sceneUUID,sceneObjectUUID,assetUUID){
        var proj = this;
        console.log("Removing Asset Instance",assetUUID,"from",sceneObjectUUID);
        this.getSceneByUUID(sceneUUID, function(scene){
            proj.getSceneObjectByUUID(scene,sceneObjectUUID,function(sceneObject){
                var indexOf = sceneObject.assetInstances.indexOf(assetUUID);
                if (indexOf > -1) {
                    sceneObject.assetInstances.splice(indexOf,1);
                } else {
                    console.log(sceneObjectUUID,": unable to remove instance of",assetUUID);
                }
            });
        });
    };

    this.getSceneObjectByUUID = function(scene,uuid,callback) {
      var ps = this;
      scene.objects.forEach(function(sceneObject) {
        ps.lookForSceneObject(sceneObject,uuid,callback);
      });
    };

    this.lookForSceneObject = function(sceneObject, uuid, callback) {
      var ps = this;
      if (sceneObject.uuid == uuid) {
        callback(sceneObject);
      } else {
        if (sceneObject.children !== undefined) {
          sceneObject.children.forEach(function(so) {
            ps.lookForSceneObject(so,uuid,callback);
          });
        }
      }
    };

    this.createScene = function() {
        var rootObject = this.createSceneObject();
        rootObject.name = "Root";
        return {
            uuid: UtilService.generateUUID(),
            name:"Untitled Scene",
            notes: "",
            objects:[rootObject],
            camera: {
                movementSpeed: 0,
                translation: { x: 0, y: 0, z: 0 },
                rotation: { x: 0, y: 0, z: 0 }
            },
            clearColour: { r:0, g:0, b:0, a:0 },
            gravity: { x:0, y:0, z:0 },
            physicsDebug: false
        };
    };

    this.createSceneObject = function() {
        return {
            uuid: UtilService.generateUUID(),
            name: "Untitled Scene Object",
            assetInstances: [],
            children: [],
            translation: { x: 0, y: 0, z: 0 },
            rotation: { x: 0, y: 0, z: 0 },
            scale: { x: 1, y: 1, z: 1 },
            translationType: this.SCENE_OBJECT_TRANSFORM_ABSOLUTE_TYPE
        };
    };

    this.createAsset = function() {
        return {
            uuid: UtilService.generateUUID(),
            name: "Untitled Asset",
        };
    };


    this.initialise = function() {
        this.project = {
            uuid:        UtilService.generateUUID(),
            name:        "Untitled Project",
            author:      "",
            description: "",
            scenes:      [this.createScene()],
            assets:      [],
            isModified:  false,
            windowSize: {
                width: this.DEFAULT_WINDOW_WIDTH,
                height: this.DEFAULT_WINDOW_HEIGHT
            }

        };
    };

    this.openProject = function(uuid, callback) {
        var svc = this;
        ApiService.readProjectFile(uuid, function(data){
            if (data === null) {
                callback(false);
            } else {
                svc.project = data;
                console.log("Opened project",svc.project);
                svc.project.isModified = false;
                callback(true);
            }
        });
    };

    this.isModified = function() {
        if (this.project !== null) return this.project.isModified;
        return false;
    };

    this.setProjectModified = function(modified) {
        this.project.isModified = modified;
    };

    this.generateDownloadBlob = function() {
        delete this.project.isModified;
        return new Blob([ JSON.stringify(this.project) ], { type : 'application/octet-stream' });
    };

    this.updateAsset = function(asset) {
        this.setAssetName(asset);
        this.setAssetType(asset);
        this.setAssetKeyFrames(asset);
    };

    this.setAssetKeyFrames = function(asset) {
      this.getAssetByUUID(asset.uuid,function(assetInProject) {
        if (assetInProject) {
          assetInProject.keyframes = asset.keyframes;
        }
      })
    };

    this.setAssetName = function(asset) {
        this.getAssetByUUID(asset.uuid,function(rsc){
          if (rsc) {
            rsc.name = asset.name;
          }
        });
    };

    this.setAssetType = function(asset) {
        this.getAssetByUUID(asset.uuid,function(rsc){
          if (rsc) {
            rsc.type = asset.type;
          }
        });
    };

    this.updateScene = function(scene) {
      this.setSceneName(scene);
    };

    this.setSceneName = function(scene) {
        this.getSceneByUUID(scene.uuid,function(projScene){
            projScene.name = scene.name;
        });
    };

    this.closeProject = function() {
        this.project = null;
    };

    this.getAssetList = function(callback) {
        callback(this.project.assets);
    };

    this.getScriptAssetList = function(callback) {
      var retval = [];
      var projSvc = this;
      this.project.assets.forEach(function(asset){
        if (asset.type == projSvc.ASSET_TYPE_SCRIPT) {
          retval.push(asset);
        }
      });
      callback(retval);
    };

    this.assetHasModelAssimp = function(uuid,callback){
        ApiService.assetExists(
            this.project.uuid,
            this.ASSET_TYPE_MODEL,
            uuid,
            this.ASSET_FORMAT_MODEL_ASSIMP,
            callback
        );
    };

    this.assetHasPhysicsObjectBvhTriangleMesh = function(uuid,callback){
        ApiService.assetExists(
            this.project.uuid,
            this.ASSET_TYPE_PHYSICS_OBJECT,
            uuid,
            this.ASSET_FORMAT_PHYSICS_OBJECT_BVH_TRIANGLE_MESH,
            callback
        );
    };

    this.assetHasModelMtl = function(uuid,callback){
        ApiService.assetExists(
            this.project.uuid,
            this.ASSET_TYPE_MODEL,
            uuid,
            this.ASSET_FORMAT_MODEL_WAVEFRONT_MTL,
            callback
        );
    };

    this.assetHasVertexShader = function(uuid,callback){
        ApiService.assetExists(
            this.project.uuid,
            this.ASSET_TYPE_SHADER,
            uuid,
            this.ASSET_FORMAT_SHADER_VERTEX,
            callback
        );
    };

    this.assetHasFragmentShader = function(uuid,callback){
      ApiService.assetExists(
        this.project.uuid,
        this.ASSET_TYPE_SHADER,
        uuid,
        this.ASSET_FORMAT_SHADER_FRAGMENT,
        callback
      );
    };

    this.assetHasAudioSfx = function(uuid,callback) {
      ApiService.assetExists(
        this.project.uuid,
        this.ASSET_TYPE_AUDIO,
        uuid,
        this.ASSET_FORMAT_AUDIO_SFX,
        callback
      );
    };

    this.assetHasAudioMusic = function(uuid,callback) {
      ApiService.assetExists(
        this.project.uuid,
        this.ASSET_TYPE_AUDIO,
        uuid,
        this.ASSET_FORMAT_AUDIO_MUSIC,
        callback
      );
    };

    this.assetHasSprite = function(uuid,callback) {
      ApiService.assetExists(
        this.project.uuid,
        this.ASSET_TYPE_SPRITE,
        uuid,
        this.ASSET_TYPE_SPRITE,
        callback
      );
    };

    this.assetHasFontTtf = function(uuid,callback) {
      ApiService.assetExists(
        this.project.uuid,
        this.ASSET_TYPE_FONT,
        uuid,
        this.ASSET_FORMAT_FONT_TRUETYPE,
        callback
      );
    };

    this.saveScriptAsset = function(asset,script,callback) {
      ApiService.uploadAsset(
        "/"+this.project.uuid+"/asset/script/"+asset.uuid+"/"+asset.format,
        script,
        callback
      );
    };

    this.saveVertexShaderAsset = function(asset,content,callback) {
      ApiService.uploadAsset(
        "/" +
        this.project.uuid +
        "/asset/" +
        this.ASSET_TYPE_SHADER +
        "/" + asset.uuid +
        "/" + this.ASSET_FORMAT_SHADER_VERTEX,
        content,
        callback
      );
    };

    this.saveFragmentShaderAsset = function(asset,content,callback) {
      ApiService.uploadAsset(
        "/" +
        this.project.uuid +
        "/asset/" +
        this.ASSET_TYPE_SHADER +
        "/" + asset.uuid + "/" +
        this.ASSET_FORMAT_SHADER_FRAGMENT,
        content,
        callback
      );
    };

    this.readAsset = function(asset,callback) {
      ApiService.readAsset(this.project, asset,callback);
    };

    this.deleteAssetData = function(asset,callback) {
      ApiService.deleteAssetData(this.project.uuid,asset.type,asset.uuid,callback);
    };

    this.createDreamAnimationKeyFrame = function (keyFrameIndex) {
        return {
          uuid          : UtilService.generateUUID(),
          name          : "KeyFrame " + keyFrameIndex,
          startTime     : 0,
          translation   : {x:0.0, y:0.0, z:0.0},
          rotation      : {x:0.0, y:0.0, z:0.0},
          scale         : {x:1.0, y:1.0, z:1.0},
          wrap          : false,
          interpolation : "none"
        };
    };

    this.removeKeyFrameFromAssetByUUID = function(assetObject, keyFremeUUID) {
      var ps = this;
        ps.getKeyFrameFromAssetByUUID(assetObject,keyFremeUUID,function(keyFrameObject) {
          ps.removeKeyFrameFromAsset(keyFrameObject,assetObject);
        });
    };

    this.getKeyFrameFromAssetByUUID = function(assetObject,keyFrameUUID,callback) {
      var retval = null;
      assetObject.keyframes.forEach(function(keyFrame) {
        if (keyFrame.uuid == keyFrameUUID) {
          retval = keyFrame;
        }
      });
      callback(retval);
    };

    this.removeKeyFrameFromAsset = function(keyFrameObject,assetObject) {
      var keyFrameIndex = assetObject.keyframes.indexOf(keyFrameObject);
      assetObject.keyframes.splice(keyFrameIndex,1);
    };

    this.getDreamAnimationInterpolationTypes = function() {
      return this.DREAM_ANIMATION_INTERPOLATION_TYPES;
    };

    this.getAllPhysicsObjectAssets = function(self) {
        var ps = this;
        return this.project.assets.filter(function(asset) {
          var retval = asset.type ==  ps.ASSET_TYPE_PHYSICS_OBJECT && asset.uuid != self.uuid;
          if (retval) {
            console.log("Found physics asset",asset.name,asset.uuid);
          }
          return retval;
        });
    };

    return this;
}]);