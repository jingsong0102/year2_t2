#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>


namespace cg
{

	struct FBO 
	{
		// Define rendering target
		enum Type {
			Default = 0,
			Texture,
			TextureHDR,
			TextureDepth,
			Deferred
		} type;

		GLuint handle;
		GLuint * pTexture;
		int width;
		int height;

		FBO(Type type = Default, int width = 0, int height = 0, GLuint* pTexture = nullptr)
			: 
			type{ type }, 
			handle{ 0 }, 
			pTexture{ pTexture },
			width{ width },
			height{ height }
		{
			resize(width, height);
		}

		void resize(int width, int height)
		{
			switch (type)
			{
			case Default:
				break;
			case Texture:
				resetTextureAsTarget(width, height, GL_RGBA8);
				break;
			case TextureHDR:
				resetTextureAsTarget(width, height, GL_RGB32F);
				break;
			case TextureDepth:
				resetTextureDepthAsTarget(width, height);
				break;
			case Deferred:
				resetDeferredAsTarget(width, height);
				break;
			}
		}

		void createTextureBuffer(int width, int height, GLenum texUnit,
			GLenum format, GLenum attachment, GLuint* pTexid = nullptr)
		{
			GLuint texid;

			glActiveTexture(texUnit);
			glGenTextures(1, &texid);
			glBindTexture(GL_TEXTURE_2D, texid);
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

			// Bind the texture to the FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texid, 0);

			if (pTexid)
				*pTexid = texid;
		}

		void createDepthBuffer(int width, int height)
		{
			GLuint tex = 0;
			GLuint depthBuf;
			glGenRenderbuffers(1, &depthBuf);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			// Bind the depth buffer to the FBO
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
		}

		void resetTextureAsTarget(int width, int height, GLenum format)
		{
			if (handle)
				glDeleteFramebuffers(1, &handle);

			if (pTexture)
				glDeleteTextures(1, pTexture);

			// Create and bind the FBO
			glGenFramebuffers(1, &handle);
			glBindFramebuffer(GL_FRAMEBUFFER, handle);

			// Create the color buffer
			createTextureBuffer(width, height, GL_TEXTURE0, format, GL_COLOR_ATTACHMENT0, pTexture);

			// Create the depth buffer
			createDepthBuffer(width, height);

			// Define an array of buffers into which outputs from 
			// the fragment shader data will be written
			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);

			// Unbind the framebuffer, and revert to default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Check for errors
			GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error(std::string("Error ") +
										std::to_string(result) + 
											": Framebuffer is not complete.\n");
		}

		void resetTextureDepthAsTarget(int width, int height)
		{
			GLuint texture = 0;

			if (handle)
				glDeleteFramebuffers(1, &handle);

			if (pTexture)
				glDeleteTextures(1, pTexture);

			GLfloat border[] = { 1.0f, 0.0f,0.0f,0.0f };

			// The depth buffer texture
			glGenTextures(1, pTexture?pTexture:&texture);
			glBindTexture(GL_TEXTURE_2D, pTexture?*pTexture:texture);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

			// Assign the depth buffer texture to texture channel 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pTexture?*pTexture:texture);

			// Create and set up the FBO
			glGenFramebuffers(1, &handle);
			glBindFramebuffer(GL_FRAMEBUFFER, handle);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pTexture?*pTexture:texture, 0);

			GLenum drawBuffers[] = { GL_NONE };
			glDrawBuffers(1, drawBuffers);

			// Unbind the framebuffer, and revert to default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Check for errors
			GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error(std::string("Error ") +
					std::to_string(result) +
					": Framebuffer is not complete.\n");
		}

		void resetDeferredAsTarget(int width, int height)
		{
			if (handle)
				glDeleteFramebuffers(1, &handle);

			// Create and bind the FBO
			glGenFramebuffers(1, &handle);
			glBindFramebuffer(GL_FRAMEBUFFER, handle);

			// Create the textures for position, normal and color
			createTextureBuffer(width, height, GL_TEXTURE0, GL_RGB32F, GL_COLOR_ATTACHMENT0);
			createTextureBuffer(width, height, GL_TEXTURE1, GL_RGB32F, GL_COLOR_ATTACHMENT1);
			createTextureBuffer(width, height, GL_TEXTURE2, GL_RGB8, GL_COLOR_ATTACHMENT2);

			// Create the depth buffer
			createDepthBuffer(width, height);

			// Define an array of buffers into which outputs from 
			// the fragment shader data will be written
			GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0,
									GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(4, drawBuffers);

			// Unbind the framebuffer, and revert to default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Check for errors
			GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error(std::string("Error ") +
					std::to_string(result) +
					": Framebuffer is not complete.\n");
		}
	};

}