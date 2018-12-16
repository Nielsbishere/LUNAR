uniform sampler2D t;
uniform vec3 ambient;

ShaderOld
	varying vec2 uv;
EndOld
ShaderNew
	in vec2 uv;
	RenderingForward
		out layout(location=0) vec4 color;								//RGB
	EndForward
	RenderingDeferred
		ERROR! Billboards are impossible with deferred rendering!
	EndDeferred
EndNew

RenderingAlbedo
EndAlbedo
RenderingLighting
	ERROR! Lighting is impossible with billboards!
EndLighting

void main(){
	RenderingForward
		RenderingAlbedo
			//Albedo lighting (No shadows / light)
			ShaderOld
				//Embedded systems EGL2.0 
				vec4 col = texture2D(t, uv);
				if(col.a <= 1.0 / 256.0)discard;
				  gl_FragColor = vec4(col.rgb * ambient, col.a);
			EndOld
			ShaderNew
				//Embedded systems EGL>=3.0 and modern OGL
				vec4 col = texture(t, uv);
				if(col.a <= 1.0 / 256.0)discard;
				  color = vec4(col.rgb * ambient, col.a);
			EndNew
		EndAlbedo
	EndForward
}