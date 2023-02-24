# Shaders guide

## How to load shaders

Your shaders are supposed to be put in the `assets/shaders` folder and stored in a `shader_name.vert` and a `shader_name.frag` file. As of now, only vertex and fragment GLSL shaders are supported.

You can load a shader like this:
```
#include <viverna/graphics/Shader.hpp>
...
auto my_shader = verna::LoadShader("shader_name");
```
However, remember to dispose of it
```
verna::FreeShader(my_shader);
```

## How to write shaders

As of now, shaders are written in `.vert` and `.frag` files inside `assets/shaders`. The syntax is plain GLSL ES 3.20, except:

- **Don't** specify *GLSL version* or *float precision*
- `SetMeshIdx()`: all vertex shaders should call this before anything else
- You can access
    - Camera properties
        - `camera.projection_matrix`
        - `camera.view_matrix`
        - `camera.pv_matrix` (projection * view)
        - WIP
    - Transform matrix
        - `MODEL_MATRIX`
    - Textures
        - `TEXTURE0`, `TEXTURE1`, ... defined by `material.textures[0]`, `material.textures[1]`, ...
    - WIP
- WIP