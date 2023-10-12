#pragma once

#include "CRenderPass.h"


struct VertexInputAttribute
{
	Uint				binding;
	Uint				location;
	VkFormat			format;
	VkVertexInputRate	input_rate;
};

struct TesselationStage
{
	VkPipelineTessellationStateCreateInfo	TesselationState;
	VkPipelineShaderStageCreateInfo			TesselationControlsStage;
	VkPipelineShaderStageCreateInfo			TesselationEvaluationStage;

	TesselationStage()
	{
		TesselationState			= {};
		TesselationControlsStage	= {};
		TesselationEvaluationStage	= {};
	}

	TesselationStage(Uint cp_count_, VkPipelineShaderStageCreateInfo tess_control_stage_, VkPipelineShaderStageCreateInfo tess_evaluation_stage_)
	{
		TesselationState.sType				= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		TesselationState.pNext				= nullptr;
		TesselationState.flags				= 0;
		TesselationState.patchControlPoints = cp_count_;

		TesselationControlsStage	= tess_control_stage_;
		TesselationEvaluationStage	= tess_evaluation_stage_;
	}
};

typedef std::vector<VertexInputAttribute> InputAttributes;


enum class PrimitiveTopology
{
	Points,
	Lines,
	Triangles,
	Quads,
	PatchList
};

enum class FaceCulling
{
	None,
	Front,
	Back,
	FrontAndBack
};

enum class PolygonMode
{
	Fill,
	Line,
	Point
};

struct AttachmentBlendingState
{
	enum BlendingOperator
	{
		Add,
		Subtract
	};

	enum BlendingFactor
	{
		Zero,
		One,
		SrcAlpha,
		DstAlpha,
		OneMinusSrcAlpha,
		OneMinusDestAlpha
	};

	

	BlendingOperator blendeingOperator;
	BlendingFactor srcFactor;
	BlendingFactor dstFactor;

	AttachmentBlendingState()//Default Constructor(Overring colors without blending)
	{
		blendeingOperator	= BlendingOperator::Add;
		srcFactor			= BlendingFactor::One;
		dstFactor			= BlendingFactor::Zero;
	}

	AttachmentBlendingState(BlendingOperator blendeingOperator, BlendingFactor srcFactor, BlendingFactor dstFactor)
	{
		this->blendeingOperator = blendeingOperator;
		this->srcFactor = srcFactor;
		this->dstFactor = dstFactor;
	}

	bool IsBlendingEnabled() const
	{
		return (blendeingOperator != BlendingOperator::Add) || (srcFactor != BlendingFactor::One) || (dstFactor != BlendingFactor::Zero); //'Blend Add One Zero' mean that we dont have to blend!(It's just override)
	}
};

typedef std::array<AttachmentBlendingState, MAXIMUM_ATTACHMENTS_PER_RENDERPASS> AttachmentsBlendingStates;

class BlendingStatesDeclarationParser
{
	private:

		static AttachmentBlendingState::BlendingOperator GetBlendingOperatorByName(std::string operatorName)
		{
			if (operatorName == "Add")
				return AttachmentBlendingState::BlendingOperator::Add;
			else if (operatorName == "Subtract")
				return AttachmentBlendingState::BlendingOperator::Subtract;

			return (AttachmentBlendingState::BlendingOperator)(0);
		};

		static AttachmentBlendingState::BlendingFactor GetBlendingFactorByName(std::string factorName)
		{
			static std::map<std::string, AttachmentBlendingState::BlendingFactor> BlendingFactorsNamesMap;
			if (BlendingFactorsNamesMap.size() == 0)
			{
				BlendingFactorsNamesMap =
				{
					{ "Zero", AttachmentBlendingState::BlendingFactor::Zero },
					{ "One",				AttachmentBlendingState::BlendingFactor::One },
					{ "SrcAlpha",			AttachmentBlendingState::BlendingFactor::SrcAlpha },
					{ "DstAlpha",			AttachmentBlendingState::BlendingFactor::DstAlpha },
					{ "OneMinusSrcAlpha",	AttachmentBlendingState::BlendingFactor::OneMinusSrcAlpha },
					{ "OneMinusDestAlpha",	AttachmentBlendingState::BlendingFactor::OneMinusDestAlpha }
				};
			}


			auto it = BlendingFactorsNamesMap.find(factorName);
			if(it != BlendingFactorsNamesMap.end())
				return it->second;
		};

	public:

		static AttachmentsBlendingStates Parse(std::string blendingStatesDeclaration)
		{
			AttachmentsBlendingStates blendingStates;

			std::vector<std::string> BlendingStatesDeclarations = SplitString(blendingStatesDeclaration, "Blend");
			for (int i = 0; i < BlendingStatesDeclarations.size(); i++)
			{
				std::vector<std::string> declarationArgs = SplitString(BlendingStatesDeclarations.at(i), " ");
				
				//Blendeing State Declaration must contain three or four args - AttachntmentIndex, BlendingOperator, srcFactor, dstFactor
				if (declarationArgs.size() == 4)
				{
					int attachntmentIndex = std::atoi(declarationArgs[0].c_str());
					if (attachntmentIndex < 0 || MAXIMUM_ATTACHMENTS_PER_RENDERPASS <= attachntmentIndex)//AttachntmentIndex must by between 0 to MAXIMUN_ATTACHMENT_PER_RENDERPASS! 
						continue;

					blendingStates[attachntmentIndex] = AttachmentBlendingState(GetBlendingOperatorByName(declarationArgs[1]), GetBlendingFactorByName(declarationArgs[2]), GetBlendingFactorByName(declarationArgs[3]));
				}
				else//If there are only 3 args then set state for all AttachntmentIndicies
				{
					AttachmentBlendingState::BlendingOperator blendingOperator = GetBlendingOperatorByName(declarationArgs[0]);
					AttachmentBlendingState::BlendingFactor srcFactor = GetBlendingFactorByName(declarationArgs[1]);
					AttachmentBlendingState::BlendingFactor dstFactor = GetBlendingFactorByName(declarationArgs[2]);
					
					for(int a = 0 ; a < MAXIMUM_ATTACHMENTS_PER_RENDERPASS ; a++)
						blendingStates[a] = AttachmentBlendingState(blendingOperator, srcFactor, dstFactor);
				}
			}
			
			return blendingStates;
		}
};

enum class ComparisonOperator
{
	Never,
	Allway,
	Less,
	Greater,
	Equal,
	NotEqual
};

struct AttachmentDepthState
{
	bool writeDepth;
	bool maskDepth;
	ComparisonOperator testingOperator;


	AttachmentDepthState(bool writeDepth = true, bool maskDepth = true, ComparisonOperator testingOperator = ComparisonOperator::Less)
	{
		this->writeDepth = writeDepth;
		this->maskDepth = maskDepth;
		this->testingOperator = testingOperator;
	}
};


struct PipelineState
{
	struct Input
	{
		PrimitiveTopology	topology = PrimitiveTopology::Triangles;
		InputAttributes		vertexAttributes;
		VkPipelineLayout	inputLayout;
		Uint				controlPointCount = 0;
	}input;


    VkPipelineShaderStageCreateInfo	vertexStage;
	VkPipelineShaderStageCreateInfo	geometricStage;
	TesselationStage				tesselationStage;

	struct Rasterization
	{
		FaceCulling faceCulling = FaceCulling::Front;
		PolygonMode polygonMode = PolygonMode::Fill;
	}rasterization;

	VkPipelineShaderStageCreateInfo	fargmentStage;

	struct Output
	{
		VkAttachmentsFormats	  colorFormats;
		VkFormat				  depthFormat;
		AttachmentsBlendingStates blendingStates;
		AttachmentDepthState	  depthState;
		CRenderPass*			  existingRenderPass;//Optional use can also use existiens Render Pass instend create a new one from formats
	
		Uint GetColorAttachntmentCount()
		{
			if (existingRenderPass != nullptr)
				return existingRenderPass->GetColorAttachmentsCount();
			else
				return colorFormats.size();
		}
	}output;
};
	



class CPipeline
{
	private:
		std::string				m_Name;

		VkPipeline				p_Pipeline;
		CRenderPass*			p_RenderPass;

		VkDevice p_Device;

	public:
		Uint ComputeStride(InputAttributes attributes_);
		CPipeline(VkDevice device_, std::string pipeline_name_, PipelineState pipeline_inf, VkBool32 present_);

		~CPipeline();

	CRenderPass* const GetRenderPass() { return p_RenderPass; }

	Void Bind(VkCommandBuffer command_buffer_)
	{
		vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, p_Pipeline);
	}
};