#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

#define SHIFT_BITS_LEFT(x)		(1 << x)
#define SHIFT_BITS_RIGHT(x)		(1 >> x)

namespace Cnek
{
	typedef std::vector<uint8_t> Container;

	class BitStream
	{
	public:
		using Iterator = std::vector<uint8_t>::iterator;
		using ConstIterator = std::vector<uint8_t>::const_iterator;
		using ReverseIterator = std::vector<uint8_t>::reverse_iterator;
		using ConstReverseIterator = std::vector<uint8_t>::const_reverse_iterator;

	public:
		BitStream() = default;
		BitStream(std::vector<uint8_t> content) : mStream(content) {}
		BitStream(std::string content) : mStream(content.begin(), content.end()) {}

		size_t Size() const { return mStream.size(); }
		size_t Length() const { return mStream.size(); }

		uint8_t* Data() { return mStream.data(); }
		const uint8_t* Data() const { return mStream.data(); }

	public:
		Iterator Begin() { return mStream.begin(); }
		Iterator begin() { return mStream.begin(); }

		Iterator End() { return mStream.end(); }
		Iterator end() { return mStream.end(); }

		ConstIterator Begin() const { return mStream.begin(); }
		ConstIterator begin() const { return mStream.begin(); }

		ConstIterator End() const { return mStream.end(); }
		ConstIterator end() const { return mStream.end(); }

		ReverseIterator BeginReverse() { return mStream.rbegin(); }
		ReverseIterator beginReverse() { return mStream.rbegin(); }

		ReverseIterator EndReverse() { return mStream.rend(); }
		ReverseIterator endReverse() { return mStream.rend(); }

		ConstReverseIterator BeginReverse() const { return mStream.rbegin(); }
		ConstReverseIterator beginReverse() const { return mStream.rbegin(); }

		ConstReverseIterator EndReverse() const { return mStream.rend(); }
		ConstReverseIterator endReverse() const { return mStream.rend(); }

	public:
		BitStream& operator=(const std::vector<uint8_t>& content);
		BitStream& operator=(std::vector<uint8_t>&& content);

		BitStream& operator=(const std::string& content);
		BitStream& operator=(std::string&& content);

	private:
		std::vector<uint8_t> mStream = {};
	};

	class Node
	{
	public:
		Node() {}
		virtual ~Node() = default;

		virtual BitStream PerformHash(BitStream text) { return text; }
		virtual BitStream PerformSymmetric(BitStream text, BitStream& key) { return text; }
		virtual BitStream PerformAsymmetric(BitStream text, BitStream& keyOne, BitStream& KeyTwo) { return text; }
	};

	class ExecutionPipeline
	{
	public:
		ExecutionPipeline() = default;
		~ExecutionPipeline() = default;

		template<class Type, class... Args>
		void RegisterNode(std::string name, Args... args)
		{
			static_assert(std::is_base_of<Node, Type>::value, "The 'Type' must be a valid derived class of 'Node'!");
			pNodes.push_back(std::make_pair(name, std::make_unique<Type>(args...)));
		}

		BitStream Execute(BitStream text);
		BitStream Execute(BitStream text, BitStream key);
		BitStream Execute(BitStream text, BitStream keyOne, BitStream keyTwo);

	private:
		std::vector<std::pair<std::string, std::unique_ptr<Node>>> pNodes = {};
	};
}