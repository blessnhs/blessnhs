#include "StdAfx.h"
#include "GSResult.h"


GSResult::GSResult(void)
{
	m_uiResultPos = m_bResult = 0;
}


GSResult::~GSResult(void)
{
}


const std::wstring& GSResult::GetValueAndNextPos()
{
	if( IsGetDataEmpty() == true )
	{
	
		printf("[*********QUERY Size Error*********]  : %s", m_Query.c_str()); 
	}

	return m_Result[ m_uiResultPos++ ];
}

bool GSResult::IsGetDataEmpty() const
{
	if( static_cast<u32>( m_Result.size() ) <= m_uiResultPos ) return true;

	return false;
}

GSResult& GSResult::operator >>( s64& data )
{
	const std::wstring&	strResult	= GetValueAndNextPos();

	if( strResult.length() == 0 )
	{
		data	= 0;
	}
	else
	{
		data	= _wtoi64( strResult.c_str() );
	}

	return *this;
}

GSResult& GSResult::operator >>( bool& data )
{
	const std::wstring&	strResult	= GetValueAndNextPos();

	if( strResult.length() == 0 )
	{
		data	= false;
	}
	else
	{
		data	= ( _wtoi( strResult.c_str() ) == 0 ? false : true );
	}

	return *this;
}

GSResult& GSResult::operator >>( std::wstring& data )
{
	data	= GetValueAndNextPos();

	return *this;
}