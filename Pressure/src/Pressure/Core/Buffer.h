#pragma once

namespace Pressure
{
	
	// Non-owning raw buffer class
	struct Buffer
	{
		std::byte* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;
		Buffer(const Buffer& other) = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		static Buffer Copy(Buffer other)
		{
			Buffer result(other.Size);
			std::memcpy(result.Data, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = new std::byte[size];
			Size = size;
		}

		void Release()
		{
			delete[] Data;
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As() const
		{
			return reinterpret_cast<T*>(Data);
		}

		operator bool() const { return Data != nullptr && Size > 0; }
	};

	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: m_Buffer(std::move(buffer))
		{
		}

		ScopedBuffer(uint64_t size)
			: m_Buffer(size)
		{
		}

		~ScopedBuffer()
		{
			m_Buffer.Release();
		}

		std::byte* Data() const { return m_Buffer.Data; }
		uint64_t Size() const { return m_Buffer.Size; }

		template<typename T>
		T* As() const
		{
			return m_Buffer.As<T>();
		}

		operator bool() const { return m_Buffer; }

	private:
		Buffer m_Buffer;
	};

}
