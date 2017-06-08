var vertexShaderText = 
[
'precision mediump float;',
' ',
'attribute vec3 vertPosition;',
'attribute vec3 vertColor;',
'varying vec3 fragColor;',
'attribute vec2 vertTextCoord;',
'varying vec2 fragTextCoord;',
'uniform mat4 m_world;',
'uniform mat4 m_view;',
'uniform mat4 m_proj;',
' ',
'void main()',
'{',
'   fragTextCoord = vertTextCoord;',
'   //fragColor = vertColor;',
'   gl_Position = m_proj * m_view * m_world * vec4(vertPosition, 1.0);',
'}',
].join('\n')

var fragmentShaderText = 
[
'precision mediump float;',
' ',
'varying vec3 fragColor;',
'varying vec2 fragTextCoord;',
'uniform sampler2D sampler;', //grabs texture from texture pointer [0]
'void main ()',
'{',
'       ',
'   gl_FragColor = texture2D(sampler, fragTextCoord);',
'}'
].join('\n')

function initDemo (){
    console.log('This is working!');
    
    var canvas = document.getElementById('graphics');
    var gl = canvas.getContext('webgl');
    if(!gl){
        gl = canvas.getContext('experimental-gl');
    }
    
    if(!gl){
        alert('Your browser doesn' + 't support WebGL');
    }
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.CULL_FACE);
    gl.frontFace(gl.CCW);
    gl.cullFace(gl.BACK);
    
    gl.viewport(0, 0, window.innerWidth, window.innerHeight);
    
    gl.clearColor(0.6, 0.8, 0.8, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
   
   //Create Shaders
   var vertShader = gl.createShader(gl.VERTEX_SHADER);
   var fragShader = gl.createShader(gl.FRAGMENT_SHADER);
   
   //Compile Vertex Shader from string variables
   gl.shaderSource(vertShader, vertexShaderText);
   gl.shaderSource(fragShader, fragmentShaderText);
   
   //Check for compiling errors
   gl.compileShader(vertShader);
   if(!gl.getShaderParameter(vertShader, gl.COMPILE_STATUS)){
        console.error('Error compiling vertex shader', gl.getShaderInfoLog(vertShader));
        return;
   }
   
   gl.compileShader(fragShader);
    if(!gl.getShaderParameter(fragShader, gl.COMPILE_STATUS)){
        console.error('Error compiling fragmentshader', gl.getShaderInfoLog(fragShader));
        return;
   }
   
   //Create Program and Link
   var program = gl.createProgram();
   gl.attachShader(program, vertShader);
   gl.attachShader(program, fragShader);
   //Linking program
   gl.linkProgram(program);
   
   //Check for Linker Errors
    if(!gl.getProgramParameter(program, gl.LINK_STATUS)){
        console.error('Error Linking Program!', gl.getProgramInfoLog(program));
        return;
    }
    
    //Testing/Development Only (Performance Hit)
    //Catches additional errors/bugs
    gl.validateProgram(program);
    if(!gl.getProgramParameter(program, gl.VALIDATE_STATUS)){
        console.error('Error Validating Program', gl.getProgramInfoLog(program));
        return;
    }
    
    // Create a Buffer for the Rasterivation of Vertex Shader
    //uses vec2 from vertex shader above
    
   var boxVertices = 
	[ // X, Y, Z           U,  V
		// Top
		-1.0, 1.0, -1.0,   0, 0,
		-1.0, 1.0, 1.0,    0, 1,
		1.0, 1.0, 1.0,     1, 1,
		1.0, 1.0, -1.0,    1, 0,

		// Left
		-1.0, 1.0, 1.0,   0, 0,
		-1.0, -1.0, 1.0,   1, 0,
		-1.0, -1.0, -1.0,  1, 1,
		-1.0, 1.0, -1.0,   0, 1,

		// Right
		1.0, 1.0, 1.0,    1, 1,
		1.0, -1.0, 1.0,   0, 1,
		1.0, -1.0, -1.0,  0, 0,
		1.0, 1.0, -1.0,   1, 0,
		
		// Front
		1.0, 1.0, 1.0,    1, 1,
		1.0, -1.0, 1.0,    1, 0,
		-1.0, -1.0, 1.0,    0, 0,
		-1.0, 1.0, 1.0,    0, 1,

		// Back
		1.0, 1.0, -1.0,     0, 0, 
		1.0, -1.0, -1.0,    0, 1,
		-1.0, -1.0, -1.0,   1, 1,
		-1.0, 1.0, -1.0,    1, 0,

		// Bottom
		-1.0, -1.0, -1.0,   1, 1,
		-1.0, -1.0, 1.0,    1, 0,
		1.0, -1.0, 1.0,     0, 0,
		1.0, -1.0, -1.0,    0, 1
	];

	var boxIndices =
	[
		// Top
		0, 1, 2,
		0, 2, 3,

		// Left
		5, 4, 6,
		6, 4, 7,

		// Right
		8, 9, 10,
		8, 10, 11,

		// Front
		13, 12, 14,
		15, 14, 12,

		// Back
		16, 17, 18,
		16, 18, 19,

		// Bottom
		21, 20, 22,
		22, 20, 23
	];


    
    //GPU mem
    var boxVertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, boxVertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(boxVertices), gl.STATIC_DRAW);
    //STATIC_DRAW = drawn once never changed, Float32 -JS must be cast in 32 bit array not 64 like c/c++
    
    var boxIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, boxIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(boxIndices), gl.STATIC_DRAW);
    
    
    var positionAttribLocation = gl.getAttribLocation(program, 'vertPosition');
    var textCoordAttribLocation = gl.getAttribLocation(program, 'vertTextCoord');
    gl.vertexAttribPointer(
        positionAttribLocation, //Attribute Location in Vertex Shader
        3, //vec2, # attributes
        gl.FLOAT, //Element type
        gl.FALSE, //Normalized Data?
        5 * Float32Array.BYTES_PER_ELEMENT, //Size of individual vertex
        0 //offsets from the beginning of vertex to this attribute
    );
     gl.vertexAttribPointer(
        textCoordAttribLocation, //Attribute Location in Vertex Shader
        2, //vec2, # texture
        gl.FLOAT, //Element type
        gl.FALSE, //Normalized Data?
        5 * Float32Array.BYTES_PER_ELEMENT, //Size of individual vertex
        3 * Float32Array.BYTES_PER_ELEMENT//offsets from the beginning of vertex to this attribute skip the x and y
    );
    
    gl.enableVertexAttribArray(positionAttribLocation);
    gl.enableVertexAttribArray(textCoordAttribLocation);
    
    //Create Texture
     var boxTexture = gl.createTexture();
     gl.bindTexture(gl.TEXTURE_2D, boxTexture);
     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        
    //load in image to tex[0]
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, document.getElementById('crate-image'));
        
    gl.bindTexture(gl.TEXTURE_2D, null); //unbind texture
    
    //Activate Program
        gl.useProgram(program);
    
    var matrWorldUniformLocation = gl.getUniformLocation(program, 'm_world');
    var matrViewUniformLocation = gl.getUniformLocation(program, 'm_view');
    var matrProjUniformLocation = gl.getUniformLocation(program, 'm_proj');
    
    var projMatrix = new Float32Array(16);
    var viewMatrix = new Float32Array(16);
    var worldMatrix = new Float32Array(16);
    mat4.identity(worldMatrix);
    
    mat4.lookAt(viewMatrix, [0,0,-12], [0,0,0], [0,1,0]);
    mat4.perspective(projMatrix, glMatrix.toRadian(45), canvas.width/canvas.height, 0.1, 1000.0);

    //Sending the matrices to shader
    gl.uniformMatrix4fv(matrWorldUniformLocation, gl.FALSE, worldMatrix);
    gl.uniformMatrix4fv(matrViewUniformLocation, gl.FALSE, viewMatrix);
    gl.uniformMatrix4fv(matrProjUniformLocation, gl.FALSE, projMatrix);

    var xRotationMatrix = new Float32Array(16);
    var yRotationMatrix = new Float32Array(16);
    

    //Main Render Loop
    //rotate the object
    var identityMatrix = new Float32Array(16);
    mat4.identity(identityMatrix);
    var angle = 0;
    var loop = function(){
        angle = performance.now() / 1000 / 6 * 2 * Math.PI;
        mat4.rotate(yRotationMatrix, identityMatrix, angle, [0,1,0]);
        mat4.rotate(xRotationMatrix, identityMatrix, angle/2.0, [1,0,0]);
        mat4.mul(worldMatrix, xRotationMatrix, yRotationMatrix);

        gl.uniformMatrix4fv(matrWorldUniformLocation, gl.FALSE, worldMatrix);
        
        gl.clearColor(0.6, 0.8, 0.8, 1.0);
        gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
        
        gl.bindTexture(gl.TEXTURE_2D, boxTexture);
        gl.activeTexture(gl.TEXTURE0);
        
        gl.drawElements(gl.TRIANGLES, boxIndices.length, gl.UNSIGNED_SHORT, 0); //uses Active bounded buffer
        //type of drawing, amount to skip, number of points(vertices)
        requestAnimationFrame(loop);
    };
    requestAnimationFrame(loop);
    

    //Rainbow the Triangle with Fragment Shader Vertex Shader -> Fragment Shader Communication
};


