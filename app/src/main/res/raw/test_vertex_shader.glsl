attribute vec2 a_Position;
attribute vec3 a_Color;
varying vec3  v_Color;

void main() {
    gl_Position =vec4(a_Position,0.0,0.0);
    v_Color=a_Color;
}
