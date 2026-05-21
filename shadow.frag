// shadow.frag

uniform sampler2DShadow texture;
varying vec4 shadow;
 
void main ()
{
  gl_FragColor = shadow + (gl_Color - shadow) * shadow2DProj(texture, gl_TexCoord[0]);
}
