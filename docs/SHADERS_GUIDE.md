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
- You may access
    - in/out vertex variables
        - `in_position`     _(input vec3)_
        - `in_tex_coords`   _(input vec2)_
        - `in_normal`       _(input vec3)_
        - `v_tex_coords`    _(output vec2)_
        - `v_normal`        _(output vec3)_
    - in/out fragment variables
        - `v_tex_coords`    _(input vec2)_
        - `v_normal`        _(input vec3)_
        - `f_color`         _(output vec4)_
    - Camera properties
        - `camera.projection_matrix`
        - `camera.view_matrix`
        - `camera.pv_matrix` _(projection * view)_
        - WIP
    - Transform matrix
        - `MODEL_MATRIX`
    - Textures
        - `TEXTURE0`, `TEXTURE1`, ... _defined by `material.textures[0]`, `material.textures[1]`, ..._
    - WIP
- WIP