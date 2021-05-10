#include "Cnek.h"

namespace Cnek
{
	BitStream& BitStream::operator=(const std::vector<uint8_t>& content)
	{
		mStream = content;
		return *this;
	}

	BitStream& BitStream::operator=(std::vector<uint8_t>&& content)
	{
		mStream = std::move(content);
		return *this;
	}

	BitStream& BitStream::operator=(const std::string& content)
	{
		mStream.clear();
		mStream.insert(mStream.end(), content.begin(), content.end());
		return *this;
	}

	BitStream& BitStream::operator=(std::string&& content)
	{
		mStream = { content.begin(), content.end() };
		return *this;
	}

	BitStream ExecutionPipeline::Execute(BitStream text)
	{
		BitStream _container = {};
		for (auto& node : pNodes)
		{
			text = node.second->PerformHash(text);
			text = node.second->PerformSymmetric(text, _container);
			text = node.second->PerformAsymmetric(text, _container, _container);
		}

		return text;
	}

	BitStream ExecutionPipeline::Execute(BitStream text, BitStream key)
	{
		for (auto& node : pNodes)
		{
			text = node.second->PerformHash(text);
			text = node.second->PerformSymmetric(text, key);
			text = node.second->PerformAsymmetric(text, key, key);
		}

		return text;
	}

	BitStream ExecutionPipeline::Execute(BitStream text, BitStream keyOne, BitStream keyTwo)
	{
		for (auto& node : pNodes)
		{
			text = node.second->PerformHash(text);
			text = node.second->PerformSymmetric(text, keyOne);
			text = node.second->PerformAsymmetric(text, keyOne, keyTwo);
		}

		return text;
	}
}