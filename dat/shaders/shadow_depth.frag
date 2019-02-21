#version 440 core


void main()
{             
	//This happens by default, 
	//By doing this it's more clear what's happening;
    gl_FragDepth = gl_FragCoord.z;

}  