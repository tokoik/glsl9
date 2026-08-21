# glsl9 - 第９回 GLSL によるシャドウマッピング サンプルプログラム

## 1. 概要

このプログラムは、OpenGL と GLSL (OpenGL Shading Language) を用いて「シャドウマッピング (Shadow Mapping)」を実装するための、学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って作成したものです。

- [第９回 GLSL によるシャドウマッピング](https://tokoik.github.io/blog/glsl%20%E5%85%A5%E9%96%80/2006/06/01/glsl.html)

光源の位置を視点にしてシーンの奥行きを描画した**デプステクスチャ（シャドウマップ）**を作成し、２回目の描画でそれを参照することによって、影の部分の陰影を求めます。固定機能によるシャドウマッピングではテクスチャ座標の自動生成機能を使い、日向と影を別々にレンダリングしていましたが、シェーダを使えばテクスチャ座標をバーテックスシェーダで求めることができ、日向と影の陰影も一度に計算できます。

![GLSL によるシャドウマッピング](https://tokoik.github.io/blog/assets/images/glsl/glsl32.jpg)

## 2. ビルド方法

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各OSとも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

> cmake-gui で設定することも可能です。その際は、`Source code path` にはプロジェクトのフォルダを指定し、`Build path` にはプロジェクトのフォルダの中に作った build というフォルダを指定してください。その後、`Configure` → `Generate` の順にクリックした後、`Open Project` をクリックすれば、開発環境が起動するはずです。

### 2.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の glsl9.sln を Visual Studio で開きます。
4. ソリューションエクスプローラーで **glsl9** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 2.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された build/glsl9.xcodeproj を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **glsl9** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 2.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 3. 使い方

### 3.1 プログラムの起動方法

- **Windows**: `build\Debug\glsl9.exe`
- **macOS**: `open build/Debug/glsl9.app` または Xcode 上で Run
- **Ubuntu Linux**: `cd build && ./glsl9`

### 3.2 操作方法

- **マウスの左ボタンでドラッグ**: シーンを 3 次元的に回転
- **キーボードの q, Q または ESC キー**: プログラムを終了

なお、シャドウマップのサイズ (512 × 512) より小さくならないよう、ウィンドウの縮小を制限しています。

## 4. 解説

このプログラムは、シェーダを使わないシャドウマッピングのサンプルプログラムをもとに、テクスチャ座標の自動生成の設定を取り除いて、GLSL のシェーダプログラム (shadow.vert, shadow.frag) による陰影付けと影付けを行うようにしたものです。シーンは scene.cpp で描いており、市松模様のタイルの上に置いた箱の周りを、球が回ります。

### 4.1 シャドウマップ用のテクスチャの準備 (`init()` 関数)

- shadow.vert（バーテックスシェーダ）と shadow.frag（フラグメントシェーダ）を読み込み・コンパイル・リンクします。
- `glGetUniformLocation(gl2Program, "texture")` により、シャドウマップのサンプラの uniform 変数の場所を `colorLoc` に取得します。
- テクスチャユニット０に 512 × 512 の `GL_DEPTH_COMPONENT` のテクスチャを確保します。このとき、以下の設定によって「テクスチャ座標の R 値とテクスチャに記録された奥行きの比較結果」をサンプリングできるようにします。

  ```cpp
  /* 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  /* もしＲの値がテクスチャの値以下なら真（つまり日向） */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  /* 比較の結果を輝度値として得る */
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
  ```

- 光源には環境光強度 `lightamb` のみを設定しておきます。直接光の強度 `lightcol` は、第２ステップの描画の直前に設定します。

### 4.2 描画手順 (`display()` 関数)

1. **第１ステップ：デプステクスチャの作成**
   - ビューポートをテクスチャのサイズに変更し、光源位置を視点としてシーンを描画します。
   - 透視変換行列は単位行列にしておき、`gluPerspective()` と `gluLookAt()` をモデルビュー変換行列に対して適用します。この行列を保存しておき、あとでテクスチャ変換行列として使います。
   - `glColorMask()` でフレームバッファへの書き込みを止め、ライティングも無効にします。また、いわゆるシャドウアクネを避けるために `glCullFace(GL_FRONT)` として背面のポリゴンの奥行きを記録します。
   - 描画したデプスバッファの内容を `glCopyTexSubImage2D()` でテクスチャメモリに転送します。

2. **第２ステップ：全体の描画**
   - 通常の視点でシーンを描画します。テクスチャ変換行列には、`[-1,1]` を `[0,1]` に収めるための変換、第１ステップで保存した行列、および現在のモデルビュー変換の逆変換の積を設定します。
   - シェーダプログラムを適用しているので、テクスチャ座標の自動生成 (`GL_TEXTURE_GEN_*`) は不要です（ソース中では `#if 0` で無効にしてあります）。
   - シャドウマップのサンプラには `glUniform1i(colorLoc, 0)` でテクスチャユニット０を指定します。

### 4.3 バーテックスシェーダ (shadow.vert)

[第２回](https://github.com/tokoik/glsl2)の Gouraud シェーディングとほぼ同じ処理を行いますが、次の３点が異なります。

<<<<<<< HEAD
- 拡散反射係数（材質の色）を `gl_FrontMaterial.diffuse` ではなく `gl_Color` から得ます（scene.cpp では `glColor*()` で色を指定しています）。また、環境光による反射光強度は影の部分にも与える必要があるので、`ambient` という `varying` 変数でフラグメントシェーダに送ります。

  ```glsl
  // 環境項の反射光強度をフラグメントシェーダに送る
  ambient = gl_LightSource[0].ambient * gl_Color;
=======
- 拡散反射係数（材質の色）を `gl_FrontMaterial.diffuse` ではなく `gl_Color` から得ます（scene.cpp では `glColor*()` で色を指定しています）。同じ色は環境光による反射光強度の計算にも必要なので、`color` という `varying` 変数でフラグメントシェーダにも送ります。

  ```glsl
  // 頂点色をフラグメントシェーダに送る
  color = gl_Color;
>>>>>>> c08e6d6 (頂点色を varying 変数でフラグメントシェーダに渡すようにする)
  ```

- 頂点のテクスチャ座標に、テクスチャ変換行列とモデルビュー変換行列を掛けた視点座標系の頂点位置を用います。

  ```glsl
  // テクスチャ座標
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_ModelViewMatrix * gl_Vertex;
  ```

<<<<<<< HEAD
- 環境光による反射光強度はフラグメントシェーダで加算するため頂点の色には含めず、拡散反射光と鏡面反射光のみを `gl_FrontColor` に設定します。
=======
- 環境光による反射光強度は、影の部分にも与える必要があるので頂点の色に含めず、フラグメントシェーダで加算します。
>>>>>>> c08e6d6 (頂点色を varying 変数でフラグメントシェーダに渡すようにする)

  ```glsl
  // 環境光強度はフラグメントシェーダで設定するので頂点の色に含めない
  gl_FrontColor = gl_LightSource[0].diffuse * gl_Color * diffuse
                + gl_LightSource[0].specular * gl_FrontMaterial.specular * specular;
  ```

### 4.4 フラグメントシェーダ (shadow.frag)

GLSL の組み込み関数 `shadow2DProj()` を使ってシャドウマップをサンプリングします。`GL_TEXTURE_COMPARE_FUNC` に `GL_LEQUAL` を設定してあるので、日向なら 1、影なら 0 が返ってきます。これをバーテックスシェーダで求めた頂点の色（拡散反射光強度＋鏡面反射光強度）の補間値 `gl_Color` に掛ければ、影の部分ではこれらが 0 になります。

<<<<<<< HEAD
そこに、影の部分にも与える環境光による反射光強度（バーテックスシェーダから `varying` 変数 `ambient` で受け取った値）を加算したものをフラグメントの色とします。これにより、日向と影を別々に描き分ける必要がなくなります。
=======
そこに、影の部分にも与える環境光による反射光強度を加えたものをフラグメントの色とします。反射係数には、バーテックスシェーダから `varying` 変数 `color` で受け取った頂点色の補間値を用います。これにより、日向と影を別々に描き分ける必要がなくなります。
>>>>>>> c08e6d6 (頂点色を varying 変数でフラグメントシェーダに渡すようにする)

```glsl
#version 120

// shadow.frag

// シャドウマップ
uniform sampler2DShadow texture;

<<<<<<< HEAD
// 環境項の反射光強度の補間値
varying vec4 ambient;
=======
// 頂点色の補間値
varying vec4 color;
>>>>>>> c08e6d6 (頂点色を varying 変数でフラグメントシェーダに渡すようにする)

void main ()
{
  // フラグメントの色
<<<<<<< HEAD
  gl_FragColor = shadow2DProj(texture, gl_TexCoord[0]) * gl_Color + ambient;
=======
  gl_FragColor = gl_LightSource[0].ambient * color
               + shadow2DProj(texture, gl_TexCoord[0]) * gl_Color;
>>>>>>> c08e6d6 (頂点色を varying 変数でフラグメントシェーダに渡すようにする)
}
```
