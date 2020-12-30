#include <iostream>

#include "IllusionEngine.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Shader.h"

using namespace illusion;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Vertex{
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};
struct Mesh{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO,VBO,EBO;
};

Mesh GetTestMesh(){
	// set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<Vertex> vertices = {
        {{0.5f,  0.5f, 0.0f}}   // top right
        ,{{0.5f, -0.5f, 0.0f}}  // bottom right
        ,{{-0.5f, -0.5f, 0.0f}} // bottom left
        ,{{-0.5f,  0.5f, 0.0f}} // top left 
    };
	
    std::vector<unsigned int> indices = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
	Mesh mesh;
	mesh.indices.insert(mesh.indices.end(),indices.begin(),indices.end());
	mesh.vertices.insert(mesh.vertices.end(),vertices.begin(),vertices.end());
	return mesh;
}
aiMesh *LoadMesh(const char * path){
	const aiScene* scene= aiImportFile(path, 
	aiProcess_CalcTangentSpace		 |
	aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);
	if(scene==NULL){
		ERR(aiGetErrorString());
		return NULL;
	}
    INFO("meshes :",scene->mNumMeshes );
	return scene->mMeshes[1];
}
Mesh ConvertToMesh(const aiMesh *aimesh){
	Mesh mesh;
    if(aimesh->HasNormals()){
        INFO("HasNormals");
    }
    if(aimesh->HasTextureCoords(0)){
        INFO("HasUvs");
    }
	for(int i=0;i<aimesh->mNumVertices; i++){
		auto current_Position =aimesh->mVertices[i];
		auto current_Normal =aimesh->mNormals[i];
		auto current_Uv =aimesh->mTextureCoords[0][i];
        Vertex vertex={
			Vec3(current_Position.x,current_Position.y,current_Position.z)
			,Vec3(current_Normal.x,current_Normal.y,current_Normal.z)
			,Vec2(current_Uv.x,current_Uv.y)
		};
		INFO(vertex.position.x,",",vertex.position.y,",",vertex.position.z);
		mesh.vertices.push_back(vertex);
	}
	for(int i=0;i<aimesh->mNumFaces; i++){
		auto current =aimesh->mFaces[i]; 
		INFO(aimesh->mFaces[i].mNumIndices);
		//only triangles supported
		mesh.indices.push_back(current.mIndices[0]);
		mesh.indices.push_back(current.mIndices[1]);
		mesh.indices.push_back(current.mIndices[2]);
	}
	return mesh;
}
void SetupMesh(Mesh& mesh){    
    glGenVertexArrays(1, &(mesh.VAO));
    glGenBuffers(1, &(mesh.VBO));
    glGenBuffers(1, &(mesh.EBO));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &(mesh.vertices[0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices[0]), GL_STATIC_DRAW);

    u32 offset=0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
    offset+=sizeof(Vec3);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
    offset+=sizeof(Vec3);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
    offset+=sizeof(Vec2);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
}
inline void RenderMesh(Mesh & m){
    glBindVertexArray(m.VAO);
    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0); // no need to unbind it every time 
}
GLuint CompileShaders();
void launchApp()
{
    Window::Create(SCR_WIDTH,SCR_HEIGHT,"Window");
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// GLuint shaderProgram=CompileShaders();
    Shader shader("../../Assets/Shaders/vshader.glsl","../../Assets/Shaders/fshader.glsl");
    //"../../Assets/Meshes/kenney_platformerkit2/Models/FBX format/block.fbx" "../../Assets/Meshes/cube.fbx"
	Mesh m=ConvertToMesh(LoadMesh("../../Assets/Meshes/block.fbx"));
	SetupMesh(m);
    Mat4x4 projection = glm::perspective(glm::radians(45.0f), (float)Window::width / (float)Window::height, 0.1f, 100.0f);
	Mat4x4 view = glm::lookAt(Vec3(0.0f,0.0f,3.0f),
                            Vec3(0.0f,0.0f,0.0f),
                            Vec3(0.0f,1.0f,0.0f));
    Mat4x4 model=Mat4x4();
    model=glm::translate(model,Vec3(0.0f,-1.0f,0.0f));
    model=glm::scale(model,Vec3(1.0f));
    INFO(m.vertices.size());
	// for(int i=0;i<m.vertices.size();i++){
	// 	INFO(m.vertices[i].position.x,m.vertices[i].position.y,m.vertices[i].position.z);
	// }
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    // -----------
    while (!Window::shouldClose)
    {
        if(Input::isKeyDown(GLFW_KEY_ESCAPE)){
            Window::Close();
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        //glUseProgram(shaderProgram);
        shader.use();
        shader.setMat4("view",view);
        shader.setMat4("projection",projection);
        shader.setMat4("model",model);

        RenderMesh(m);
        //INFO(m.vertices[0].position.x);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(Window::glfwWindow);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &(m.VAO));
    glDeleteBuffers(1, &(m.VBO));
    glDeleteBuffers(1, &(m.EBO));
    // glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    Window::Destroy();
}

GLuint CompileShaders(){
int vertexShader = glCreateShader(GL_VERTEX_SHADER);
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
	// build and compile our shader program
    // ------------------------------------
    // vertex shader
    
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	return shaderProgram;
}