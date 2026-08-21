#version 120

// shadow.frag

// シャドウマップ
uniform sampler2DShadow texture;

// 頂点色の補間値
varying vec4 color;

void main ()
{
  // フラグメントの色
  gl_FragColor = gl_LightSource[0].ambient * color
               + shadow2DProj(texture, gl_TexCoord[0]) * gl_Color;
}
