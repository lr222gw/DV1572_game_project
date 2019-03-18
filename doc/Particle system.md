Particle system:

Computation options: GPU-computed particle logic or CPU-side logic.

Pros of CPU alternative: easier to for the engine developer and engine users to exchange custom particle logic.

Pros of GPU: more suited for highly parallelizable code.

We made the construct accept a lambda wrapping the logic and a set of textures (albedo, specular, normal, emission) for the particle billboards. The lambda gets free access to particle data and can modify the particle values (lifetime, position, colour, scale factor, etc).

Implementation options:
   Using instancing (decent compatibility & efficient).
   Using geometry shaders (limited compatibility).
   A VBO with all the particles (most compatible).

Issues to be aware of:
  Transparency; easiest work around is to sort the particles back-to-front and draw them in that order since due to their billboard nature the only cases where overlapping is an issue is when the distance of two billboards is identical and they overlap within their plans, which could lead to minor Z-fighting.

  However, they can still intersect in very noticeable manners with other geometry (fom the regular geometry pass). One potential way of dealing with this is to blend the current particle fragment based off of its proximity to the depth buffer at the same pixel.

  Animated and "multi-textured" particle systems can be realized by implementing a sheet of textures--a texture atlas. That way the frame number or local texture ID can easily be converted into a sub-rectangle within the atlas.

  Another way of achieving multiple textures is to just create one particle system for each texture variant and then put them in the same location.

Limitations:
  The maximum particle count (per system) is hard-coded due to the need to pre-allocate the memory when loading it. However, this can be circumvented in a somewhat hacky way by simple creating more particle systems in the same location with the same textures and logic.

  A current limitation is that we had to add the following check in the particle system fragment shader to by-pass a minor issue:

  "if (g_albedo.w < 0.525) discard;"

  This is because we use the alpha data for certain texture channels to encode additional data (such as specular intensity for the specular textures). But this issue is easy to fix by simply changing the shaders to use the value of the RGB parts of the texture both for colour and intensity.

Elaborate on: partition


///////////////////////////////////////////////////////////////////////////////////

Front-to-back rendering:

In order to accomplish this acceleration all that is needed is to maintain a sorted order of the model instances that is based off of their distance from the viewport's camera position.

Depending on one's need, a quad tree or octree would be an ideal data structure for storing them. Then it's just a question of traversing the tree based off of the camera position.

Even for 3D games, many games have a 2D layout  (that is, having the X and Z axes be the longest, while the Z axis is short).

The easiest way to achieve this, however, is to just store references to the instances in a vector and then use STD's sort algorithm with a custom lambda that compares two instance's distances from the viewport, with the latter being passed via the lambda capture.

One small optimization that we did is to have our SceneManager class hand our model instances a callback function that they call everytime they get transformed. If a change happens, a flag is set that frame to sort the instances before iterating them for drawing.

Of course, this will also need to be called if the viewport changes its position as well.

The drawback with this naïve vector method in our case is that we keep all instances in one vector. Ideally, we'd like to have a separate container for instances within proximity.
The predicate for this could either be a simple function that implements a distance threshold to determine eligibility; or if a level-of-detail system is used, the LoD could also factor in.
Then it also depends on what kind of game the engine is to be used for. For a game like Minecraft, the octree approach would be ideal; for a real time stategy game, a quad tree or cell grid system (where each cell contains a set of instances) would be good.

When it comes to our particle system, however, we don't use front-to-back rendering but rather back-too-front rendering in order to handle transparency in a non-complicated manner.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Backface culling

Compare the triangle normal (in the geometry shader) to the camera's facing direction to determine whether it is facing towards or away from the beholder.

Using the backface culling built into OpenGL is both easier and gives better result.

TODO, flesh out.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Deferred Shading:

We created a G-buffer with the following channels:
Albedo
Specular
Normal
Emission
Position
Displacement
...
