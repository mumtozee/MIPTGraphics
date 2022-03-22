#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow *window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>

void DrawTriangles(GLuint vbuffer, GLuint programID, GLint first,
                   GLuint matrixID, GLfloat *mvp) {
  glUseProgram(programID);
  glUniformMatrix4fv(matrixID, 1, GL_FALSE, mvp);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)nullptr);
  glDrawArrays(GL_TRIANGLES, first, 3);
  glDisableVertexAttribArray(0);
}

glm::mat4 ComputeMVPFromTime(double time, glm::vec3 initialPos = glm::vec3(0, 1, 0)) {
  glm::vec3 newPos = initialPos + glm::vec3(glm::cos(time), 0, glm::sin(time));
  glm::mat4 Projection =
      glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);
  glm::mat4 View =
      glm::lookAt(newPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 MVP = Projection * View * Model;
  return MVP;
}

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(800, 400, "Tutorial 01", nullptr, nullptr);

  if (window == nullptr) {
    fprintf(stderr, "Some error occured");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

  GLuint vArrayId[2];
  glGenVertexArrays(2, vArrayId);
  glBindVertexArray(vArrayId[0]);
  glBindVertexArray(vArrayId[1]);

  GLuint programID_1 = LoadShaders("VertexShader.vertexshader",
                                   "FragmentShaderA.fragmentshader");
  GLuint programID_2 = LoadShaders("VertexShader.vertexshader",
                                   "FragmentShaderB.fragmentshader");

  GLuint matrixID_1 = glGetUniformLocation(programID_1, "MVP");
  GLuint matrixID_2 = glGetUniformLocation(programID_2, "MVP");

  static const GLfloat g_vertex_buffer_data[] = {
      -0.8f, -0.8f, 0.0f, 0.4f, -0.8f, 0.0f, -0.4f, 0.8f,  0.0f,
      -0.4f, -0.8f, 0.0f, 0.4f, 0.8f,  0.0f, 0.8f,  -0.8f, 0.0f,
  };

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
               g_vertex_buffer_data, GL_STATIC_DRAW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  double time = 0.0f;
  do {
    glClear(GL_COLOR_BUFFER_BIT);
    time = glfwGetTime();
    glm::mat4 MVP = ComputeMVPFromTime(time);
    DrawTriangles(vertexbuffer, programID_1, 0, matrixID_1, &MVP[0][0]);
    DrawTriangles(vertexbuffer, programID_2, 3, matrixID_2, &MVP[0][0]);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  glDeleteBuffers(1, &vertexbuffer);
  glDeleteVertexArrays(2, vArrayId);
  glDeleteProgram(programID_2);
  glDeleteProgram(programID_1);
  glfwTerminate();
}
