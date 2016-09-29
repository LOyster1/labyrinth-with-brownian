#version 430

attribute vec4 vPosition;
attribute vec4 vColor;

out  vec4 color;

uniform    mat4 modelview;


void main() 
{ 
  gl_Position = modelview*vPosition;
  color = vColor;
}