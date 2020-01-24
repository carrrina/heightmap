#include "Tema3.h"

#include <vector>
#include <string>
#include <iostream>
#include <stb/stb_image.h>

#include <Core/Engine.h>

using namespace std;

Tema3::Tema3()
{
}

Tema3::~Tema3()
{
}

void Tema3::Init()
{
	thirdPersonCamera = false;

	heightPixels = stbi_load("Source/Laboratoare/Tema3/Textures/heightmap.png", &width, &height, &channels, STBI_grey);

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema3/Textures/heightmap.png", GL_REPEAT);
		mapTextures["heightmap"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Laboratoare/Tema3/Textures/ground2.jpg", GL_REPEAT);
		mapTextures["ground"] = texture;
	}

	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Shader* shader = new Shader("ShaderTema3");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	camera = new Tema::Camera();
	camera->Set(glm::vec3(-5, 10, -5), glm::vec3(-5, 5, 5), glm::vec3(0, 1, 0));

	{
		lightPosition = glm::vec3(0, 1, 1);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 80;
		materialKd = 0.5;
		materialKs = 0.5;
		cutoff_angle = 50;
		angleOX = 0;
		angleOY = 0;
		typeOfLight = 0;
	}
}

void Tema3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::Update(float deltaTimeSeconds)
{
	glm::vec3 p = glm::vec3(1, 0, 2);
	for (int i = p.x - 0.5; i < p.x + 0.5; i++)
		for (int j = p.z - 0.5; j < p.z + 0.5; j++)
		{
			int row = (i + 110) * 256 / 220;
			int col = (j + 110) * 256 / 220;
			int index = row * width + col;
			//if (newHeightPixels[index] > 1)
				//newHeightPixels[index]--;
		}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -50, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 1, 10));
		RenderSimpleMesh(meshes["plane"], shaders["ShaderTema3"], modelMatrix, mapTextures["heightmap"], mapTextures["ground"]);
	}
}

void Tema3::FrameEnd()
{
}

void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	if (typeOfLight == 1)
	{
		lightPosition = camera->GetEyePosition();
		lightDirection = camera->GetForwardVector();
	}

	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	glm::vec3 eyePosition = camera->GetEyePosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int type = glGetUniformLocation(shader->program, "type_of_light");
	glUniform1i(type, typeOfLight);

	int cut_off_angle = glGetUniformLocation(shader->program, "cut_off_angle");
	glUniform1f(cut_off_angle, cutoff_angle);

	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)newHeightPixels);
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "heigthMap"), 0);
	}

	if (texture2)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "colorMap"), 1);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	float cameraSpeed = 2.0f;

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !thirdPersonCamera)
	{
		if (window->KeyHold(GLFW_KEY_W)) {
			camera->MoveForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			camera->MoveForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}
	else
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = camera->GetRightVector();
		glm::vec3 forward = camera->GetEyePosition();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * cameraSpeed;
		if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * cameraSpeed;
		if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * cameraSpeed;
		if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * cameraSpeed;
		if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * cameraSpeed;
		if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * cameraSpeed;

		if (window->KeyHold(GLFW_KEY_I)) angleOX += deltaTime * 5;
		if (window->KeyHold(GLFW_KEY_J)) angleOY += deltaTime * 5;
		if (window->KeyHold(GLFW_KEY_K)) angleOX -= deltaTime * 5;
		if (window->KeyHold(GLFW_KEY_L)) angleOY -= deltaTime * 5;


		if (window->KeyHold(GLFW_KEY_Z)) cutoff_angle += deltaTime * 20;
		if (window->KeyHold(GLFW_KEY_X)) cutoff_angle -= deltaTime * 20;

		glm::mat4 turn = glm::mat4(1);
		turn = glm::rotate(turn, angleOY, glm::vec3(0, 1, 0));
		turn = glm::rotate(turn, angleOX, glm::vec3(1, 0, 0));

		lightDirection = glm::vec3(0, -1, 0);
		lightDirection = glm::vec3(turn * glm::vec4(lightDirection, 0));
	}
}

void Tema3::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F)
		typeOfLight = 1 - typeOfLight;
}

void Tema3::OnKeyRelease(int key, int mods)
{
}

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (!thirdPersonCamera) {
			camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
			camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
		}
	}
}

void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema3::OnWindowResize(int width, int height)
{
}
