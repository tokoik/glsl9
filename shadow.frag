#version 120

// shadow.frag

// シャドウマップ
uniform sampler2DShadow texture;

// 環境項の反射光強度の補間値
varying vec4 ambient;

void main ()
{
  // フラグメントの色
  gl_FragColor = shadow2DProj(texture, gl_TexCoord[0]) * gl_Color + ambient;
}
