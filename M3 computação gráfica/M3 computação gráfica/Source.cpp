﻿/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 11/04/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

//stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Nossa classe que armazena as infos dos shaders
#include "Shader.h"

const float Pi = 3.1419;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções 
void leOBJ(string path);
void leMTL(string path);
int setupSprite();
int loadTexture(string path);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//variaveis globais
bool rotateX = false, rotateY = false, rotateZ = false;
std::vector< GLfloat > triangulos = {};
string MTLname = "";
string TexturePath = "";
//using namespace glm;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M3 Rafael Brustolin Comp. Gráfica", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Compilando e buildando o programa de shader
	Shader shader("../Shaders/sprite.vs", "../Shaders/sprite.fs");

	//leitura do arquivo obj
	leOBJ("../Objects/SuzanneTriTextured.obj");

	//leitura do arquivo mtl
	leMTL("../Objects/" + MTLname);

	//Carregando uma textura e armazenando o identificador na memória	
	GLuint texID = loadTexture("../Textures/" + TexturePath);


	// Gerando uma geometria de quadrilátero com coordenadas de textura
	GLuint VAO = setupSprite();

	// Ativando o shader
	glUseProgram(shader.ID);

	// Associando com o shader o buffer de textura que conectaremos
	// antes de desenhar com o bindTexture
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	//Criando a matriz de projeção usando a GLM
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1000.0, 1000.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, false, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Limpa o buffer de cor
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glLineWidth(10);
		glPointSize(20);

		//Criando a matriz de modelo usando a GLM
		glm::mat4 model = glm::mat4(1); //matriz identidade
		float angle = (GLfloat)glfwGetTime()/10;

		model = glm::translate(model, glm::vec3(400.0, 300.0, 100));
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}		
		model = glm::scale(model, glm::vec3(200.0, 200.0, 200.0));
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		//Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, (triangulos.size() / 8));

		glBindVertexArray(0); //unbind - desconecta

		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}
}

void leOBJ(string path) {
	//leitura do arquivo
	std::vector< glm::vec3 > vertexIndices = {};
	std::vector< glm::vec2 > textureIndices = {};

	//leitura do arquivo
	string line, v, t, valuesX, valuesY, valuesZ;
	int i = 0;

	ifstream myfile(path);
	while (!myfile.eof()) {
		getline(myfile, line);

		std::istringstream iss(line);

		if (line.find("mtllib") == 0) {
			iss >> v >> MTLname;
		}

		if (line[0] == 'v' && line[1] == ' ') {
			iss >> v >> valuesX >> valuesY >> valuesZ;
			glm::vec3 aux = { std::stof(valuesX), std::stof(valuesY), std::stof(valuesZ) };
			vertexIndices.push_back(aux);
		}

		if (line[0] == 'v' && line[1] == 't') {
			iss >> v >> valuesX >> valuesY;
			glm::vec2 aux = { std::stof(valuesX), std::stof(valuesY) };
			textureIndices.push_back(aux);
		}

		if (line[0] == 'f' && line[1] == ' ')
		{
			string delimiter = " ";
			string delimiter1 = "/";

			//pos vertices
			int primeira_parte_vertex = 0;
			int segunda_parte_vertex = 0;
			int terceira_parte_vertex = 0;

			//pos texturas
			int primeira_parte_textures = 0;
			int segunda_parte_textures = 0;
			int terceira_parte_textures = 0;

			auto finish = line.find(delimiter); // gives position of first occurrence of delimiter
			line = line.substr(finish + 1);

			if (finish != line.npos) { // find returned the end of string
				primeira_parte_vertex = stoi(line.substr(0, line.find(delimiter1))); //
				finish = line.find(delimiter1); // gives position of first occurrence of delimiter
				line = line.substr(finish + 1);
				primeira_parte_textures = stoi(line.substr(0, line.find(delimiter1)));
			}

			finish = line.find(delimiter); // gives position of first occurrence of delimiter
			line = line.substr(finish + 1);

			if (finish != line.npos) { // find returned the end of string
				segunda_parte_vertex = stoi(line.substr(0, line.find(delimiter1))); // 
				finish = line.find(delimiter1); // gives position of first occurrence of delimiter
				line = line.substr(finish + 1);
				segunda_parte_textures = stoi(line.substr(0, line.find(delimiter1))); // 
			}

			finish = line.find(delimiter); // gives position of first occurrence of delimiter
			line = line.substr(finish + 1);

			if (finish != line.npos) { // find returned the end of string
				terceira_parte_vertex = stoi(line.substr(0, line.find(delimiter1))); // 
				finish = line.find(delimiter1); // gives position of first occurrence of delimiter
				line = line.substr(finish + 1);
				terceira_parte_textures = stoi(line.substr(0, line.find(delimiter1))); // 
			}

			triangulos.push_back(vertexIndices[primeira_parte_vertex - 1][0]);
			triangulos.push_back(vertexIndices[primeira_parte_vertex - 1][1]);
			triangulos.push_back(vertexIndices[primeira_parte_vertex - 1][2]);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(textureIndices[primeira_parte_textures - 1][0]);
			triangulos.push_back(textureIndices[primeira_parte_textures - 1][1]);

			triangulos.push_back(vertexIndices[segunda_parte_vertex - 1][0]);
			triangulos.push_back(vertexIndices[segunda_parte_vertex - 1][1]);
			triangulos.push_back(vertexIndices[segunda_parte_vertex - 1][2]);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(textureIndices[segunda_parte_textures - 1][0]);
			triangulos.push_back(textureIndices[segunda_parte_textures - 1][1]);

			triangulos.push_back(vertexIndices[terceira_parte_vertex - 1][0]);
			triangulos.push_back(vertexIndices[terceira_parte_vertex - 1][1]);
			triangulos.push_back(vertexIndices[terceira_parte_vertex - 1][2]);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(0.2f);
			triangulos.push_back(textureIndices[terceira_parte_textures - 1][0]);
			triangulos.push_back(textureIndices[terceira_parte_textures - 1][1]);
		}
	}
	myfile.close();
}
void leMTL(string path) {
		//leitura do arquivo
		string line, v;

		ifstream myfile(path);
		while (!myfile.eof()) {
			getline(myfile, line);

			std::istringstream iss(line);

			if (line.find("map_Kd") == 0) {
				iss >> v >> TexturePath;
			}
		}
		myfile.close();
}

int setupSprite()
{
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, triangulos.size()*(sizeof(GLfloat)), triangulos.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	// Primeiro atributo - Layout 0 - posição - 3 valores - x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// Segundo atributo - Layout 1 - cor - 3 valores - r, g, b
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Terceiro atributo - Layout 2 - coordenadas de textura - 2 valores - s, t (ou u, v)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //desvincula
	//glBindVertexArray(0); //desvincula

	return VAO;
}

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{	
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}