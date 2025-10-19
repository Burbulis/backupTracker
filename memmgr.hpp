#ifndef __MEMMGR__
#define __MEMMGR__
#include <vector>
#include <iterator>
#include "base.h"


#define NOTHROW throw(std::)
/*
	Зарефакторить всё это говно к херам, убрать new вообще!

*/



class mem_mgr
{
	
    std::vector<char> dataBuffer;
	//size_t size_;
	bool blocked;
	public:

	enum{NPOS = 10001001};

	mem_mgr():blocked(false) 
	{}

	void
	setBlock(void)
	{
		blocked = true;
	}

	void
	unBlock(void)
	{
		blocked = false;
	}

	bool
	isBlocked(void) const 
	{
		return (blocked);
	}

	bool
	reinit(size_t size)
	{
		if(blocked)
			return (false);
		
		TRACE_ARG(size);
		blocked = true;
		dataBuffer.clear();
		dataBuffer.resize(size);
		memset( dataBuffer.data(),0,dataBuffer.size());
		blocked = false;
		return (true);
	}

	bool
	reinit()
	{
		if(blocked)
			return (false);	
		TRACE();
		memset( dataBuffer.data(),110,dataBuffer.size());
		return (true);
	}

	size_t
	size() const noexcept
	{
		TRACE();
		return (dataBuffer.size());
	}
	
	char *ptr() 
	{
		TRACE();
		return (dataBuffer.data()); 
	}

	std::vector<char>::iterator begin()
	{
		TRACE();
		return (dataBuffer.begin());
	}

	std::vector<char>::iterator end()
	{
		TRACE();
		return (dataBuffer.end());
	}

	bool set(char *buffer, size_t size)
	{
		if(blocked)
			return (false);

		TRACE_ARG(size);

		if (size <= this->size())
			memcpy(dataBuffer.data(),buffer,size);
		else 
			throw std::runtime_error("buffers size mismach!");	
		
		return (true);
	}

/*
bufferType -любой тип с буффером data, количество байт для записи берётся из аттрибутов самого этогго типа:
bufferType::SIZE_OF_BLOCKS // размер всех блоков в буффере вместе с заголовком.
  Иногда требуется взять из буффера по заданному смещению begin_position 
  буффер и на его основе подготовить faBuffer -тот буффер с которым работает 
  reader производящий запись в таблицы посредством  sqlInsert.
  этот тип данных остался с самого первого MVP этого продукта.
    И совсем избавиться от него было довольно сложно. Да и зачем избавляться от того,
   что  Нормально рботает?
  */
	template
	<
		typename bufferType
	>
	bool set(std::unique_ptr<bufferType>& faBuffer,size_t begin_position = 0)
	{		
		if(blocked)
			return (false);
		TRACE_ARG(begin_position);
		memcpy(reinterpret_cast<char *>(&faBuffer->data[0]),&ptr()[begin_position],bufferType::SIZE_OF_BLOCKS);
		return (true);
	}

	bool set(std::vector<char>& buff)
	{
		if(blocked)
			return (false);
		TRACE();
		buff.resize(dataBuffer.size()+1);
		memcpy(&buff[0],&dataBuffer[0],dataBuffer.size());
		return (true);
	}	

/*
Этот метод создаёт и готовит объект типа buffertype , 
на основе существующего буффера с учётом начальной позиции beginposition /обычно pos = 0/
*/
	template
	<
		typename bufferType
	>
	std::unique_ptr<bufferType> make(size_t begin_position)
	{	
		TRACE_ARG(begin_position);
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"++make()");
		_LOG::out_s << "bufferType::SIZE_OF_BLOCKS = " << bufferType::SIZE_OF_BLOCKS <<" begin_position = " << begin_position << "bufferType::BUFFER_SIZE = " << bufferType::BUFFER_SIZE << " size() =" << size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,_LOG::out_s);
		std::unique_ptr<bufferType> faBuffer = std::make_unique<bufferType>();	
		memcpy(&faBuffer->data[0],&dataBuffer[begin_position],bufferType::BUFFER_SIZE);
		faBuffer->buff.md.size = bufferType::SIZE_OF_BLOCKS;
		_LOG::out_s << " begin_position = " << begin_position << "bufferType::BUFFER_SIZE = " << bufferType::BUFFER_SIZE << " size() =" << size() << std::endl;
		LOGTOFILE(LOGHTML::messageType::STRONG_WARNING,"--make()");
		return (faBuffer);
	}



	template
	<
		typename Seq
	>
	size_t
	findSeq(const Seq& token)
	{
		TRACE_ARG(token)
		std::vector<char>::iterator iter =  std::search(dataBuffer.begin(),dataBuffer.end(),token.begin(),token.end());
		if (iter!=dataBuffer.end())
			return (std::distance(dataBuffer.begin(),iter));
		return (NPOS);
	}

	bool init(const std::vector<char>& buffer)
	{
		if(blocked)
			return (false);

		TRACE_ARG(buffer.size());
		reinit(buffer.size());
		memcpy(&dataBuffer[0],&buffer[0],buffer.size());
		return (true);
	}

	std::vector<char>&
	getMem(void) 	
	{
		TRACE();
		return (dataBuffer);
	} 

	~mem_mgr()
	{
		TRACE();
	}

};
#endif