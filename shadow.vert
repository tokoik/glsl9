#version 120

// shadow.vert

// 環境項の反射光強度
varying vec4 ambient;

void main()
{
  // 頂点のクリッピング座標値
  gl_Position = ftransform();

  // テクスチャ座標
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_ModelViewMatrix * gl_Vertex;

  // 視点座標系の頂点の位置
  vec4 position = gl_ModelViewMatrix * gl_Vertex;

  // 視点座標系の法線ベクトル
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);

  // 視点座標系の光線ベクトル
  vec3 light = normalize((gl_LightSource[0].position * position.w
    - gl_LightSource[0].position.w * position).xyz);

  // 拡散反射率
  float diffuse = max(dot(light, normal), 0.0);

  // 視点座標系の視線ベクトル
  vec3 view = -normalize(position.xyz);

  // 視点座標系の中間ベクトル
  vec3 halfway = normalize(light + view);

  // 鏡面反射率
  float specular = pow(max(dot(normal, halfway), 0.0),
    gl_FrontMaterial.shininess);

  // 環境項の反射光強度をフラグメントシェーダに送る
  ambient = gl_LightSource[0].ambient * gl_Color;

  // 環境光強度はフラグメントシェーダで設定するので頂点の色に含めない
  gl_FrontColor = gl_LightSource[0].diffuse * gl_Color * diffuse
                + gl_LightSource[0].specular * gl_FrontMaterial.specular * specular;
}
