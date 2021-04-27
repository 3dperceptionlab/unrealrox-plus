# Python API Documentation

## List actors
- **`actor_list()`**
    - This command outputs the list of the actor names in the scene (*Actor* children). It also returns a string list with those names.
- **`object_list()`**
    - This command outputs the list of the static-object names in the scene  (*Actor* children with *StaticMeshComponent*). It also returns a string list with those names.
- **`skeletal_list()`**
    - This command outputs the list of the skeleton names in the scene (*Actor* children with *SkeletalMeshComponent*).It also returns a string list with those names.
- **`camera_list()`**
    - This command outputs the list of the camera names in the scene (*Actor* children with *CameraComponent*). It also returns a string list with those names.
## Move, Rotate, Scale actors
- **`move(string actor_name, float X, float Y, float Z)`**
    - Moves the actor (Skeletal or Static) `actor_name` immediately to `X,Y,Z` position in world coordinates.
- **`move(string actor_name, float X, float Y, float Z, float secs)`**
    - Moves the actor (Skeletal or Static) `actor_name` to `X,Y,Z` position in world coordinates gradually, throughout `secs` seconds.
- **`rotate(string actor_name, float P, float Y, float R)`**
    - Rotates the actor (Skeletal or Static) `actor_name` immediately by `P,Y,R` degrees (euler angles).
- **`rotate(string actor_name, float P, float Y, float R, float secs)`**
    - Rotates the actor (Skeletal or Static) `actor_name` by `P,Y,R` degrees (euler angles), throughout `secs` seconds.
- **`scale(string actor_name, float rateX, float rateY, float rateZ)`**
    - Scales the actor (Skeletal or Static) `actor_name` by `rateX,rateY,rateZ` rates. Each dimension can be specified separately, but the three values must be the same if proportions are wanted to be kept.
- **`(X, Y, Z) = get_location(string actor_name)`**
    - Returns the world coordinates `X,Y,Z` with the position of the actor `actor_name`.
- **`(P, Y, R) = get_rotation(string actor_name)`**
    - Returns the euler angles `P,Y,R` which express the rotation of the actor `actor_name`.
- **`(rateX, rateY, rateZ) = get_scale(string actor_name)`**
    - Returns the rates `rateX,rateY,rateZ` with the scale of the actor `actor_name`.
## Skeletons
- **`socket_list(string sk_name)`**
    - Lists the socket names of the `sk_name` skeleton. It also returns a string list with those names.
- **`move_socket(string sk_name, string socket_name, float X, float Y, float Z)`**
    - Moves the socket `socket_name` of the skeleton `sk_name` to `X, Y, Z` position in world coordinates.
- **`move_socket(string sk_name, string socket_name, float X, float Y, float Z, float secs)`**
    - Moves gradually the socket `socket_name` of the skeleton `sk_name` to `X, Y, Z` position in world coordinates, throughout `secs` seconds.
- **`rotate_socket(string sk_name, string socket_name, float P, float Y, float R)`**
    - Roatates the socket `socket_name` of the skeleton `sk_name` by `P, Y, R` degrees (euler angles) in world space.
- **`rotate_socket(string sk_name, string socket_name, float P, float Y, float R, float secs)`**
    - Roatates gradually the socket `socket_name` of the skeleton `sk_name` by `P, Y, R` degrees (euler angles) in world space, throughout `secs` seconds.
- **`(X, Y, Z) = get_socket_location(string sk_name, string socket_name)`**
    - Returns the world coordinates `X,Y,Z` with the position of the actor `actor_name`.
- **`(P, Y, R) = get_socket_rotation(string sk_name, string socket_name)`**
    - Returns the euler angles `P,Y,R` (in world space) which express the rotation of the actor `actor_name`.


## Assets, Textures and Spawn actors
- **`set_asset_path(string path)`**
    - Sets the folder specified in `path` as the one to look for assets. The default path is empty, which means that the folder to check is *Content*. The path is always relative to the *Content* folder inside the Unreal project folder, and the format must be `/folder/subfolder` (slash at the begining and not at the end).
- **`asset_list()`**
    - This command lists the names of the assets on the current asset path. It also returns a string list with those names.
- **`change_texture (string actor_name, string asset_name)`**
    - Change the texture of the actor `actor_name` (must be a *MeshActor*) to `asset_name` (must be a texture). The texture asset must be placed at the current asset path.
- **`spawn_actor(string actor_name, string asset_name, float X, float Y, float Z)`**
    - Spawns the actor on the asset `asset_name` on the position `X,Y,X` on world coordinates with name  `actor_name`.
## Cameras
- **`spawn_camera(string camera_name, float X, float Y, float Z)`**
    - Spawns a *CameraActor* in the scene with the name `camera_name`.
- **`camera_look_at(string camera_name, string actor_name)`**
    - Rotates the camera `camera_name` to point to the location in which the actor `actor_name` is located.
- **`camera_look_at(string camera_name,float X, float Y, float Z)`**
    - Rotates the camera `camera_name` to `X,Y,Z` location in world coordinates.
## UnrealROX
- **`set_camera_stereo(string camera_name, bool is_stereo, float baseline)`**
    - Sets the camera `camera_name` to stereo camera (if `is_stereo` is *true*) or to mono camera (if `is_stereo` is *false*). If `is_stereo` is true, the stereo pair will be defined with the specified `baseline`. The position and rotation of the pair will be refered to the middle point between them, as they will be created to be in the same plane. 
- **`bool = is_camera_stereo(string camera_name)`**
    - Returns *true* if camera `camera_name` is stereo, and *false* if it isn’t.
- **`set_output_frames_resolution(int width, int height)`**
    - Sets the resolution of the output frames to `width x height`. Default is 1920 x 1080.
- **`set_output_frames_path(string path)`**
    - Sets the folder specified in `path` as the one to save the output frames. The default path is TODO. The path is always relative to the *Content* folder inside the Unreal project folder, and the format must be TODO.
- **`toggle_scene_physics(bool enabled)`**
    - Enables and disables (if `enabled` is *true*/*false*) the gravity and physics simulations on the scene. Useful to “freeze” the scene.
- **`bool = is_scene_physics_enabled()`**
    - Returns *true* or *false* depending on physics are enabled or disabled.
- **`get_rgb(string camera_name)`**
    - Get the RGB frame of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/rgb` subfolder with default name (TODO).
- **`get_rgb(string camera_name, string file_name)`**
    - Get the RGB frame of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside `camera_name/rgb` subfolder with the name `file_name`.
- **`get_depth(string camera_name)`**
    - Get the depth map of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/depth` subfolder with default name (TODO).
- **`get_depth(string camera_name, string file_name)`**
    - Get the depth map of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/depth` subfolder with the name `file_name`.
- **`get_normal(string camera_name)`**
    - Get normal map of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/normal` subfolder with default name (TODO).
- **`get_normal(string camera_name, string file_name)`**
    - Get normal map of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/normal` subfolder with the name `file_name`.
- **`get_instance_mask(string camera_name)`**
    - Get instance segmentation mask of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/mask` subfolder with default name (TODO).
- **`get_instance_mask(string camera_name, string file_name)`**
    - Get instance segmentation mask of the current scene from the point of view of `camera_name` and saves it in the output frames path, inside`camera_name/mask` subfolder with the name `file_name`.
- **`(vertex list) = get_3d_bounding_box(string actor_name)`**
    - Returns a list with eight X,Y,Z points in world coordinates that represent the vertexes of the 3D orientes bounding box of the actor `actor_name` (must be an *Actor* with *StaticMeshComponent*).
