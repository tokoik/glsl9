#version 120

// shadow.frag

// シャドウマップ
uniform sampler2DShadow texture;
 
void main ()
{
  // フラグメントの色
  gl_FragColor = gl_LightSource[0].ambient * gl_FrontMaterial.ambient
               + shadow2DProj(texture, gl_TexCoord[0]) * gl_Color;
}
