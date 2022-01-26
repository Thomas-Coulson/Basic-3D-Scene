#include "Renderer.h"
#include "Camera.h"
#include "ImageLoader.h"

Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_VAO);
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
GLuint Renderer::CreateProgram(std::string vsPath, std::string fsPath)
{
	//This function has been edited to make a program based on passedin shaders

	// Create a new program (returns a unqiue id)
	GLuint program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, vsPath) };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, fsPath) };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	/*if (!Helpers::LinkProgramShaders(m_program))
		return 0;*/

	if (!Helpers::LinkProgramShaders(program))
		return 0;

	return program;
}


// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into different programs
	m_program = CreateProgram("Data\\Shaders\\vertex_shader.vert", "Data\\Shaders\\fragment_shader.frag");
	m_cubeProgram = CreateProgram("Data\\Shaders\\cube_vertex_shader.vert", "Data\\Shaders\\cube_fragment_shader.frag");
	m_skyProgram = CreateProgram("Data\\Shaders\\sky_vertex_shader.vert", "Data\\Shaders\\sky_fragment_shader.frag");

	//==================================================================================================================================================================
	//make skybox
	Model skyModel;
	skyModel.modelName = "skybox";

	int textureNumber = 0;

	//load skybox model
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\Sky\\Clouds\\skybox.x")) {
		return false;
	}

	//loop through all of the mesh in the model:
	for (const Helpers::Mesh& mesh : loader.GetMeshVector()) {
		std::vector<glm::vec3> vertices = mesh.vertices;
		std::vector<glm::vec3> normals = mesh.normals;
		std::vector<GLuint> elements = mesh.elements;
		std::vector<glm::vec2> texCoords = mesh.uvCoords;

		Mesh newMesh;

		//VBOs
		//positions
		GLuint positionsVBO;
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//normals
		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//texture coordinates
		GLuint textCoordsVBO;
		glGenBuffers(1, &textCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//elements
		newMesh.numElements = elements.size();
		GLuint elementsEBO;
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		//VAOs
		//positons
		glGenVertexArrays(1, &newMesh.vao);
		glBindVertexArray(newMesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		//normals
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		//texture
		glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		//elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBindVertexArray(0);

		//Texture loading
		Helpers::ImageLoader Imageloader2;

		//load a texture for each face of teh skybox model 
		switch (textureNumber) {
		case 0:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Top.tga")) {
				return false;
			}
			break;
		case 1:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Right.tga")) {
				return false;
			}
			break;
		case 2:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Left.tga")) {
				return false;
			}
			break;
		case 3:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Front.tga")) {
				return false;
			}
			break;
		case 4:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Back.tga")) {
				return false;
			}
			break;
		case 5:
			if (!Imageloader2.Load("Data\\Models\\Sky\\Clouds\\SkyBox_Bottom.tga")) {
				return false;
			}
			break;
		default:
			break;
		}

		
		glGenTextures(1, &newMesh.tex);
		glBindTexture(GL_TEXTURE_2D, newMesh.tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Imageloader2.Width(), Imageloader2.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Imageloader2.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		skyModel.meshVector.emplace_back(newMesh);

		textureNumber++;

	}

	//==================================================================================================================================================================
	//make cube
	Model cube;
	cube.modelName = "Cube";

	Mesh cubeMesh;

	//make cube vertecies
	std::vector<glm::vec3> cubeVertices = {
		glm::vec3(-5.0f, -5.0f, -5.0f),//v1 a
		glm::vec3(5.0f, -5.0f, -5.0f),//v2 b 
		glm::vec3(-5.0f, 5.0f, -5.0f),//v3 c
		glm::vec3(5.0f, 5.0f, -5.0f),//v4 d 
		glm::vec3(5.0f, 5.0f, -5.0f),//v5 d 
		glm::vec3(5.0f, -5.0f, -5.0f),//v6 b 
		glm::vec3(5.0f, -5.0f, 5.0f),//v7 h
		glm::vec3(5.0f, 5.0f, 5.0f),//v8 f
		glm::vec3(5.0f, 5.0f, 5.0f),//v9 f
		glm::vec3(5.0f, -5.0f, 5.0f),//v10 h 
		glm::vec3(-5.0f, -5.0f, 5.0f),//v11 g
		glm::vec3(-5.0f, 5.0f, 5.0f),//v12 e
		glm::vec3(-5.0f, 5.0f, 5.0f),//v13 e
		glm::vec3(-5.0f, -5.0f, 5.0f),//v14 g 
		glm::vec3(-5.0f, -5.0f, -5.0f),//v15 a
		glm::vec3(-5.0f, 5.0f, -5.0f),//v16 c
		glm::vec3(-5.0f, -5.0f, -5.0f),//v17 a
		glm::vec3(-5.0f, -5.0f, 5.0f),//v18 g 
		glm::vec3(5.0f, -5.0f, 5.0f),//v19 h
		glm::vec3(5.0f, -5.0f, -5.0f),//v20 b
		glm::vec3(-5.0f, 5.0f, 5.0f),//v21 e
		glm::vec3(-5.0f, 5.0f, -5.0f),//v22 c 
		glm::vec3(5.0f, 5.0f, -5.0f),//v23 d
		glm::vec3(5.0f, 5.0f, 5.0f),//v24 f

	};

	//make cube colours
	std::vector<glm::vec3> cubeColours = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.65f, 0.0f),
		glm::vec3(1.0f, 0.65f, 0.0f),
		glm::vec3(1.0f, 0.65f, 0.0f),
		glm::vec3(1.0f, 0.65f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};

	//make cube elements
	std::vector<GLuint> cubeElements = {
		2, 1, 0,
		3, 1, 2,
		6, 5, 4,
		7, 6, 4,
		10, 9, 8,
		11, 10, 8,
		14, 13, 12,
		15, 14, 12,
		18, 17, 16,
		19, 18, 16,
		22, 21, 20,
		23, 22, 20
	};

	//positions
	GLuint cubePositionsVBO;
	glGenBuffers(1, &cubePositionsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubePositionsVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* cubeVertices.size(), cubeVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//colours
	GLuint cubeColoursVBO;
	glGenBuffers(1, &cubeColoursVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeColoursVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* cubeColours.size(), cubeColours.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	cubeMesh.numElements = cubeElements.size();

	GLuint cubeElementsEBO;
	glGenBuffers(1, &cubeElementsEBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementsEBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* cubeElements.size(), cubeElements.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//positions
	glGenVertexArrays(1, &cubeMesh.vao);

	glBindVertexArray(cubeMesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, cubePositionsVBO);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//colours

	glBindBuffer(GL_ARRAY_BUFFER, cubeColoursVBO);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementsEBO);

	glBindVertexArray(0);

	cubeMesh.translation = glm::vec3(-10, 20, -170);

	cube.meshVector.emplace_back(cubeMesh);


	//==================================================================================================================================================================
	//make terrain

	Model terrain;
	terrain.modelName = "Terrain";

	Mesh newMesh;

	//defines dimentions of terrain
	int numCellsX{ 150 };
	int numCellsZ{ 150 };

	int numVertsX{ numCellsX + 1 };
	int numVertsZ{ numCellsZ + 1 };
	int numVerts{ numVertsX * numVertsZ };


	//==================================================================================================================================================================
	std::vector<glm::vec3> positions;
	//set positions
	for (int i = 0; i < numVertsZ; i++) {
		for (int j = 0; j < numVertsX; j++) {
			positions.push_back(glm::vec3(j * 3, 0, -i * 3));
		}
	}

	newMesh.translation = glm::vec3(-65, -2, 70);

	//==================================================================================================================================================================
	//heightmap loading
	Helpers::ImageLoader Imageloader;
	if (!Imageloader.Load("Data\\Heightmaps\\Test.png")) {
		return false;
	}

	float vertexXtoImage = (float)Imageloader.Width() / numVertsX;
	float vertexZtoImage = (float)Imageloader.Height() / numVertsZ;

	GLbyte* imageData = (GLbyte*)Imageloader.GetData();

	//set height of positions to image data in the heightmap
	for (int z = 0; z < numVertsZ; z++) {

		int imageZ = vertexZtoImage * z;

		for (int x = 0; x < numVertsX; x++) {
			int imageX = vertexXtoImage * x;

			size_t offset = ((size_t)imageX + (size_t)imageZ * Imageloader.Width()) * 4;
			BYTE height = imageData[offset];
			int myvec = (z * numVertsX) + x;

			positions[myvec].y = ((float)height / 10) - 4;

		}
	}

	//==================================================================================================================================================================

	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> verticies;


	std::vector<glm::vec2> texCoords;
	//set texCoords
	for (int i = 0; i < numVertsZ; i++) {
		for (int j = 0; j < numVertsZ; j++) {
			texCoords.push_back(glm::vec2(j / (float)numVertsX, i / (float)numVertsZ));
		}
	};

	std::vector<GLuint> elements;

	bool diamondToggle = true;

	//set elements (in diamond pattern)
	for (int cellZ = 0; cellZ < numCellsZ; cellZ++) {
		for (int cellX = 0; cellX < numCellsX; cellX++) {
			int startVertIndex = cellZ * numVertsX + cellX;

			if (diamondToggle) {
				//first triangle
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX);

				//second triangle
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);
			}
			else {
				//first triangle
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);

				//second triangle
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);
			}
			diamondToggle = !diamondToggle;
		}
		diamondToggle = !diamondToggle;
	}


	//set normals

	normals.resize(numVerts);
	verticies.resize(numVerts);

	for (int e = 0; e < elements.size(); e += 3) {
		verticies[0] = positions[elements[e]];
		verticies[1] = positions[elements[(size_t)e + 1]];
		verticies[2] = positions[elements[(size_t)e + 2]];

		glm::vec3 edge1 = verticies[1] - verticies[0];
		glm::vec3 edge2 = verticies[2] - verticies[0];

		normals[elements[e]] += glm::cross(edge1, edge2);
		normals[elements[(size_t)e + 1]] += glm::cross(edge1, edge2);
		normals[elements[(size_t)e + 2]] += glm::cross(edge1, edge2);
	}

	for (int n = 0; n < normals.size(); n++) {
		glm::normalize(normals[n]);
	}


	//VBOs
	//positions
	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* positions.size(), positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normals
	GLuint normalsVBO;
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint textCoordsVBO;
	glGenBuffers(1, &textCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//elements
	newMesh.numElements = elements.size();
	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//VAOs
	//positons
	glGenVertexArrays(1, &newMesh.vao);
	glBindVertexArray(newMesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//texture
	glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBindVertexArray(0);

	//Texture loading
	Helpers::ImageLoader Imageloader1;
	if (!Imageloader1.Load("Data\\Textures\\ocean.jpg")) {
		return false;
	}

	glGenTextures(1, &newMesh.tex);
	glBindTexture(GL_TEXTURE_2D, newMesh.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Imageloader1.Width(), Imageloader1.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Imageloader1.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	terrain.meshVector.push_back(newMesh);


	//==================================================================================================================================================================
	//list of all file names for meshes
	std::vector<std::string> aquaPigMeshes = 
	{ "Data\\Models\\AquaPig\\hull.obj",
		"Data\\Models\\AquaPig\\wing_right.obj",
		"Data\\Models\\AquaPig\\wing_left.obj",
		"Data\\Models\\AquaPig\\propeller.obj",
		"Data\\Models\\AquaPig\\gun_base.obj",
		"Data\\Models\\AquaPig\\gun.obj" };

	Model newModel;
	newModel.modelName = "aquaPig";

	for (std::string fileName : aquaPigMeshes) {
		//load aqua pig
		Helpers::ModelLoader loader;
		if (!loader.LoadFromFile(fileName)) {
			return false;
		}


		//now we can loop through all of the mesh in the model:
		for (const Helpers::Mesh& mesh : loader.GetMeshVector()) {
			std::vector<glm::vec3> vertices = mesh.vertices;
			std::vector<glm::vec3> normals = mesh.normals;
			std::vector<GLuint> elements = mesh.elements;
			std::vector<glm::vec2> texCoords = mesh.uvCoords;

			Mesh newMesh;

			//VBOs
			//positions
			GLuint positionsVBO;
			glGenBuffers(1, &positionsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//normals
			GLuint normalsVBO;
			glGenBuffers(1, &normalsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLuint textCoordsVBO;
			glGenBuffers(1, &textCoordsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//elements
			newMesh.numElements = elements.size();
			GLuint elementsEBO;
			glGenBuffers(1, &elementsEBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


			//VAOs
			//positons
			glGenVertexArrays(1, &newMesh.vao);
			glBindVertexArray(newMesh.vao);
			glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);

			//normals
			glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(
				1,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);

			//texture
			glBindBuffer(GL_ARRAY_BUFFER, textCoordsVBO);
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(
				2,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0
			);

			//elements
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
			glBindVertexArray(0);

			//set data in mesh struct based on each mesh
			if (fileName == "Data\\Models\\AquaPig\\hull.obj") {
				newMesh.translation = glm::vec3(0, 0, 0);
				newMesh.rotation = glm::vec3(-0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\hull.obj";
			}
			else if (fileName == "Data\\Models\\AquaPig\\wing_right.obj") {
				newMesh.translation = glm::vec3(-2.231, 0.272, -2.663);
				newMesh.rotation = glm::vec3(-0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\wing_right.obj";
			}
			else if (fileName == "Data\\Models\\AquaPig\\wing_left.obj") {
				newMesh.translation = glm::vec3(2.231, 0.272, -2.663);
				newMesh.rotation = glm::vec3(-0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\wing_left.obj";
			}
			else if (fileName == "Data\\Models\\AquaPig\\propeller.obj") {
				newMesh.translation = glm::vec3(0, 0.695, -3.816);
				newMesh.rotation = glm::vec3(glm::half_pi<float>() - 0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\propeller.obj";
			}
			else if (fileName == "Data\\Models\\AquaPig\\gun_base.obj") {
				newMesh.translation = glm::vec3(0, 0.569, -1.866);
				newMesh.rotation = glm::vec3(-0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\gun_base.obj";
			}
			else if (fileName == "Data\\Models\\AquaPig\\gun.obj") {
				newMesh.translation = glm::vec3(0, 2.026, -1.214);
				newMesh.rotation = glm::vec3(-0.174533, 0, 0);
				newMesh.name = "Data\\Models\\AquaPig\\gun.obj";
			}

			//Texture loading
			Helpers::ImageLoader Imageloader;
			if (!Imageloader.Load("Data\\Models\\AquaPig\\aqua_pig_2K.png")) {
				return false;
			}

			glGenTextures(1, &newMesh.tex);
			glBindTexture(GL_TEXTURE_2D, newMesh.tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Imageloader.Width(), Imageloader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Imageloader.GetData());
			glGenerateMipmap(GL_TEXTURE_2D);

			newModel.meshVector.emplace_back(newMesh);

		}

	}

	//push all models onto modelVector
	modelVector.emplace_back(skyModel);
	modelVector.emplace_back(cube);
	modelVector.emplace_back(terrain);
	modelVector.emplace_back(newModel);


	return true;

}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 1500.0f);


	glm::mat4 model_xform = glm::mat4(1);

	static float angle1 = 0;
	static bool rotateY = true;

	// Bind our VAO and render

	static float angle = 0;

	//render each mesh in each model
	for (Model& model : modelVector) {
		for (Mesh& mesh : model.meshVector) {

			//use different render conditions based on each model
			if (model.modelName == "skybox") {
				
				glDepthMask(GL_FALSE);
				glDisable(GL_DEPTH_TEST);

				// Compute camera view matrix and combine with projection matrix for passing to shader
				glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
				glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
				glm::mat4 combined_xform = projection_xform * view_xform2;

				// Use our program. Doing this enables the shaders we attached previously.
				glUseProgram(m_skyProgram);

				// Send the combined matrix to the shader in a uniform
				GLuint combined_xform_id = glGetUniformLocation(m_skyProgram, "combined_xform");
				glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));


			}
			else if (model.modelName == "Cube") {
				glDepthMask(GL_TRUE);
				glEnable(GL_DEPTH_TEST);

				// Compute camera view matrix and combine with projection matrix for passing to shader
				glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
				glm::mat4 combined_xform = projection_xform * view_xform;

				// Use our program. Doing this enables the shaders we attached previously.
				glUseProgram(m_cubeProgram);

				// Send the combined matrix to the shader in a uniform
				GLuint combined_xform_id = glGetUniformLocation(m_cubeProgram, "combined_xform");
				glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

				model_xform = glm::translate(model_xform, mesh.translation);

				if (rotateY) // Rotate around y axis		
					model_xform = glm::rotate(model_xform, angle1, glm::vec3{ 0 ,1,0 });
				else // Rotate around x axis		
					model_xform = glm::rotate(model_xform, angle1, glm::vec3{ 1 ,0,0 });

				angle1 +=0.003f;

				if (angle1 > glm::two_pi<float>())
				{
					angle1 = 0;
					rotateY = !rotateY;
				}

			}
			else if (model.modelName != "skybox" && model.modelName != "Cube") {
				glDepthMask(GL_TRUE);
				glEnable(GL_DEPTH_TEST);

				// Compute camera view matrix and combine with projection matrix for passing to shader
				glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
				glm::mat4 combined_xform = projection_xform * view_xform;

				// Use our program. Doing this enables the shaders we attached previously.
				glUseProgram(m_program);

				// Send the combined matrix to the shader in a uniform
				GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
				glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

				model_xform = glm::translate(model_xform, mesh.translation);
				model_xform = glm::rotate(model_xform, mesh.rotation.x, glm::vec3{ 1, 0, 0 });
				model_xform = glm::rotate(model_xform, mesh.rotation.y, glm::vec3{ 0, 1, 0 });
				model_xform = glm::rotate(model_xform, mesh.rotation.z, glm::vec3{ 0, 0, 1 });

				if (mesh.name == "Data\\Models\\AquaPig\\propeller.obj") {
					model_xform = glm::rotate(model_xform, angle, glm::vec3{ 0, 1, 0 });
					angle += 0.02f;
				}
			}

			

			GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
			glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.tex);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

			model_xform = glm::mat4(1);	
			glBindVertexArray(mesh.vao);
			glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);
		}

	}

}

