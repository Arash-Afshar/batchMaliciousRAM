#pragma once
#include "Wire.h"
#include <vector>
#include "Common/Defines.h"
#include <array>

namespace libBDX {
	enum class GateType
	{
		Zero = 0, //0000,
		Nor = 1, //0001
		nb_And = 2, //0010
		nb = 3, //0011
		na_And = 4, //0100
		na = 5, //0101
		Xor = 6, //0110
		Nand = 7, //0111
		And = 8, //1000
		Nxor = 9, //1001
		a = 10,//1010
		nb_Or = 11,//1011
		b = 12,//1100
		na_Or = 13,//1101
		Or = 14,//1110
		One = 15 //1111
	};



	struct Gate
	{
		u8 eval(u64 i) const
		{
			return ((u8)mType & (1 << i))? 1 : 0;
		}

		Gate(u64 input0, u64 input1, u64 output, GateType gt)
		{
			mInput = { { input0, input1 } };
			mType = gt;
			//mLgicTable =
			//{ {
			//	static_cast<u8>(static_cast<u8>(gt) & static_cast<u8>(1)),
			//	static_cast<u8>(static_cast<u8>(gt) & static_cast<u8>(2)),
			//	static_cast<u8>(static_cast<u8>(gt) & static_cast<u8>(4)),
			//	static_cast<u8>(static_cast<u8>(gt) & static_cast<u8>(8))
			//} };
			mOutput = output;


			// compute the gate modifier variables
			mAAlpha = (gt == GateType::Nor || gt == GateType::na_And || gt == GateType::nb_Or || gt == GateType::Or);
			mBAlpha = (gt == GateType::Nor || gt == GateType::nb_And || gt == GateType::na_Or || gt == GateType::Or);
			mCAlpha = (gt == GateType::Nand || gt == GateType::nb_Or || gt == GateType::na_Or || gt == GateType::Or);
		}

		//// returns the gate type i.e. and, or, ...
		//inline GateType Type() const
		//{
		//	return mType;// (GateType)(
		//		//(mLgicTable[0] ? 1 : 0) |
		//		//(mLgicTable[1] ? 2 : 0) |
		//		//(mLgicTable[2] ? 4 : 0) |
		//		//(mLgicTable[3] ? 8 : 0));
		//}

		// truth table padded to be 64 bits
		//std::array<u8, 4> mLgicTable;
		std::array<u64, 2> mInput;
		u64 mOutput;
		inline const GateType& Type() const { return mType; }
		inline const u8& AAlpha() const { return mAAlpha; }
		inline const u8& BAlpha() const { return mBAlpha; }
		inline const u8& CAlpha() const { return mCAlpha; }
	private:
		GateType mType;
		u8 mAAlpha, mBAlpha, mCAlpha;
	};


	template<u32 tableSize>
	struct GarbledGate// : public Gate
	{
	public:
		std::array<block, tableSize> mGarbledTable;
		//GarbledGate(const Gate& gate)
		//   : Gate(gate)
		//{}
	};
}