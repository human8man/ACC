xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 23;
 0.00000;0.05000;0.00000;,
 0.03536;0.03536;0.00000;,
 0.00000;0.03536;-0.03536;,
 0.00000;0.05000;0.00000;,
 0.00000;0.03536;0.03536;,
 0.00000;0.05000;0.00000;,
 -0.03536;0.03536;0.00000;,
 0.00000;0.05000;0.00000;,
 0.00000;0.03536;-0.03536;,
 0.05000;0.00000;0.00000;,
 0.00000;0.00000;-0.05000;,
 0.00000;0.00000;0.05000;,
 -0.05000;0.00000;0.00000;,
 0.00000;0.00000;-0.05000;,
 0.03536;-0.03536;0.00000;,
 0.00000;-0.03536;-0.03536;,
 0.00000;-0.03536;0.03536;,
 -0.03536;-0.03536;-0.00000;,
 0.00000;-0.03536;-0.03536;,
 0.00000;-0.05000;0.00000;,
 0.00000;-0.05000;0.00000;,
 0.00000;-0.05000;0.00000;,
 0.00000;-0.05000;0.00000;;
 
 16;
 3;0,1,2;,
 3;3,4,1;,
 3;5,6,4;,
 3;7,8,6;,
 4;2,1,9,10;,
 4;1,4,11,9;,
 4;4,6,12,11;,
 4;6,8,13,12;,
 4;10,9,14,15;,
 4;9,11,16,14;,
 4;11,12,17,16;,
 4;12,13,18,17;,
 3;15,14,19;,
 3;14,16,20;,
 3;16,17,21;,
 3;17,18,22;;
 
 MeshMaterialList {
  1;
  16;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.286275;0.105882;0.054902;1.000000;;
   100.000000;
   1.000000;1.000000;1.000000;;
   0.286275;0.105882;0.054902;;
  }
 }
 MeshNormals {
  17;
  0.000000;1.000000;0.000000;,
  0.620172;0.665013;-0.416107;,
  0.416107;0.665013;-0.620172;,
  0.416107;0.665013;0.620172;,
  -0.620172;0.665013;0.416107;,
  0.707107;0.000000;-0.707107;,
  0.678598;0.281085;0.678598;,
  0.707107;0.000000;0.707107;,
  -0.678598;0.281085;0.678598;,
  -0.707107;0.000000;0.707107;,
  -0.678598;0.281085;-0.678598;,
  -0.707107;0.000000;-0.707107;,
  0.613953;-0.496108;-0.613953;,
  0.613953;-0.496108;0.613953;,
  -0.613953;-0.496108;0.613953;,
  -0.613953;-0.496108;-0.613953;,
  0.000000;-1.000000;0.000000;;
  16;
  3;0,1,2;,
  3;0,3,1;,
  3;0,4,3;,
  3;0,2,4;,
  4;2,1,5,5;,
  4;6,3,7,7;,
  4;8,4,9,9;,
  4;10,10,11,11;,
  4;5,5,12,12;,
  4;7,7,13,13;,
  4;9,9,14,14;,
  4;11,11,15,15;,
  3;12,12,16;,
  3;13,13,16;,
  3;14,14,16;,
  3;15,15,16;;
 }
 MeshTextureCoords {
  23;
  0.125000;0.000000;,
  0.250000;0.250000;,
  0.000000;0.250000;,
  0.375000;0.000000;,
  0.500000;0.250000;,
  0.625000;0.000000;,
  0.750000;0.250000;,
  0.875000;0.000000;,
  1.000000;0.250000;,
  0.250000;0.500000;,
  0.000000;0.500000;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  1.000000;0.500000;,
  0.250000;0.750000;,
  0.000000;0.750000;,
  0.500000;0.750000;,
  0.750000;0.750000;,
  1.000000;0.750000;,
  0.125000;1.000000;,
  0.375000;1.000000;,
  0.625000;1.000000;,
  0.875000;1.000000;;
 }
}
