#pragma once

#include <string>
#include <list>
#include <map>

class FieldValue
{
public:
	typedef enum _FieldType{
		TString = 0,
		TInt,
		TFloat,
		TArray,
		TDouble,
		TObject,
	}FieldType;

	//class ObjectPair{
	//public:
	//	std::string key;
	//	FieldValue val;
	//};

private:
	FieldType m_ft;
	std::string m_string;
	std::list<FieldValue> m_array;
	int m_int;
	float m_float;
	double m_double;
	std::map<std::string, FieldValue> m_object;

public:
	FieldValue(){}
	FieldValue(const char* p)
	{
		m_ft = TString;
		m_string = p;
	}

	FieldValue(int v){
		m_ft = TInt;
		m_int = v;
	}

	FieldValue(double v){
		m_ft = TDouble;
		m_double = v;
	}

	FieldValue(FieldType t)
	{
		m_ft = t;
	}

	FieldValue(const FieldValue& v){
		m_ft = v.m_ft;
		m_string = v.m_string;
		for (auto i = v.m_array.begin(); i != v.m_array.end(); i++){
			m_array.push_back(*i);
		}
		m_object = v.m_object;
		m_int = v.m_int;
		m_double = v.m_double;
	}

	FieldValue& operator=(const FieldValue& v)
	{
		m_ft = v.m_ft;
		m_string = v.m_string;
		for (auto i = v.m_array.begin(); i != v.m_array.end(); i++){
			m_array.push_back(*i);
		}
		m_object = v.m_object;
		m_int = v.m_int;
		m_double = v.m_double;

		return *this;
	}

	FieldType getType() const{
		return m_ft;
	}

	std::list<FieldValue> getArray(){
		return m_array;
	}

	int getInt(){
		return m_int;
	}

	std::string getString(){
		return m_string;
	}

	float getFloat(){
		return m_float;
	}

	double getDobule(){
		return m_double;
	}

	bool push(FieldValue fv){
		if (TArray == m_ft)
		{
			m_array.push_back(fv);
			return true;
		}

		return false;
	}

	bool setString(const char* str)
	{
		if (TString == m_ft)
		{
			m_string = str;
			return true;
		}
		return false;
	}

	bool setInt(int v){
		if (TInt == m_ft){
			m_int = v;
			return true;
		}
		return false;
	}

	bool setKeyVal(const char* key, FieldValue& v)
	{
		if (TObject == m_ft)
		{
			std::string k = key;
			m_object[k] = v;
			return true;
		}
		return false;
	}

	std::map<std::string, FieldValue> getObject()
	{
		return m_object;
	}
};