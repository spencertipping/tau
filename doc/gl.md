# GL rendering
```sh
$ bin/sigma-fast 'n1 p||-
  "b"
  ('\''p "main"
    "attribute vec3 a_position;
     void main()
     {
       gl_Position = vec4(a_position,1);
     }"
    "void main()
     {
       gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
     }")
  ('\''v "foo" ((0 3)) (0.5 0.5,0) (0.5,-0.5,0) (-0.5 0.5,0))
  "e";
  K gl"" p>Hx'
```
