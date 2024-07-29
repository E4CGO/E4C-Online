#pragma once

// writer Matsumura

template< class T>
class Liner8TreeManager		// 線形8分木配列
{
public:
	template< class T>
	struct Cell;

	template< class T>
	struct OFT	//OBJECT_FOR_TREE構造体
	{
		Cell<T>* m_pCell = nullptr;//登録空間
		T* m_pObject = nullptr;//判定対象オブジェクト
		OFT<T>* m_pPre = nullptr;//前のOFTオブジェクト
		OFT<T>* m_pNext = nullptr;//次のOFTオブジェクト
		bool Remove()//リストから除外
		{
			// すでに逸脱している時は処理終了
			if (!m_pCell)
				return false;

			// 自分を登録している空間に自身を通知
			if (!m_pCell->OnRemove(this))
				return false;

			// 逸脱処理
			// 前後のオブジェクトを結びつける
			if (m_pPre != nullptr)
			{
				m_pPre->m_pNext = m_pNext;
			}
			if (m_pNext != nullptr)
			{
				m_pNext->m_pPre = m_pPre;
			}

			m_pPre = nullptr;
			m_pNext = nullptr;
			m_pCell = nullptr;
			return true;
		}
		bool Clear();//単純削除
	};

	template< class T>
	struct Cell
	{
		OFT<T>* pLatest = nullptr;//最新OFTオブジェクトのアドレス
		int m_iIndex = 0;//この空間の配列番号
		bool Push(OFT<T>* pOFT)//OFTオブジェクト登録
		{
			if (!pOFT)
				return false;

			if (pOFT->m_pCell == this)	// ２重登録チェック
				return false;

			if (!pLatest)
			{
				pLatest = pOFT;	// 空間に新規登録
			}
			else
			{
				// 最新OFTオブジェクトを更新
				pOFT->m_pNext = pLatest;
				pLatest->m_pPre = pOFT;
				pLatest = pOFT;
			}

			// 登録したOFTオブジェクトにこの空間を登録
			pOFT->m_pCell = this;
			return true;
		}
		bool OnRemove(OFT<T>* pOFT)//最新OFTオブジェクトを除外
		{
			if (!pOFT)
				return false;

			if (pOFT == pLatest)
			{
				pLatest = pOFT->m_pNext;	// 最新OFTオブジェクトをずらす
			}
			return true;
		}
	};

	//線形8分木配列を構築
	bool Initialize(unsigned int Level, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		// 設定最高レベル以上の空間は作れない
		if (Level > MAXLEVEL)
			return false;

		// 各レベルでの空間数を算出
		m_iPow[0] = 1;
		for (int i = 1; i < MAXLEVEL + 1; i++)
		{
			m_iPow[i] = m_iPow[i - 1] * 8;
		}

		// Level回分割（0基点）の配列作成
		m_iCellNum = (m_iPow[Level] * 8 - 1) / 7;
		ppCellAry = new Cell<T>*[m_iCellNum];
		ZeroMemory(ppCellAry, sizeof(Cell<T>*) * m_iCellNum);

		// 有効領域を登録
		m_fLeft = minX;
		m_fBottom = minY;
		m_fFront = minZ;
		m_fW = maxX - minX;
		m_fH = maxY - minY;
		m_fD = maxZ - minZ;
		m_fUnit_W = m_fW / (1 << Level);
		m_fUnit_H = m_fH / (1 << Level);
		m_fUnit_D = m_fD / (1 << Level);
		m_uiLevel = Level;

		return true;
	}

	//終了化
	void Finalize()
	{		
		for (int i = 0; i < m_iCellNum; i++)
		{
			if (ppCellAry[i] != nullptr)
			{
				OFT<T>* pOFT = ppCellAry[i]->pLatest;
				while (pOFT != nullptr)
				{
					OFT<T>* next_pOFT = pOFT->m_pNext;
					delete pOFT->m_pObject; // OFT内のオブジェクトを削除
					delete pOFT; // 生成したOFTオブジェクトを削除
					pOFT = next_pOFT;
				}

				delete ppCellAry[i]; // 生成した空間を削除
			}
		}

		delete[] ppCellAry;	// 線形空間配列削除
	}

	//オブジェクト登録
	bool Regist(float pos_minX, float pos_maxX, float pos_minY, float pos_maxY, float pos_minZ, float pos_maxZ, T* pObj)
	{
		// オブジェクトの境界範囲から登録モートン番号を算出
		int Elem = GetLinerIndex(pos_minX, pos_maxX, pos_minY, pos_maxY, pos_minZ, pos_maxZ);

		if (Elem < 0)
			return false;// 登録失敗

		OFT<T>* pOFT = new OFT<T>;
		pOFT->m_pObject = pObj;

		if (Elem < m_iCellNum) {
			// 空間が無い場合は新規作成
			if (!ppCellAry[Elem])
				CreateNewCell(Elem);
			return ppCellAry[Elem]->Push(pOFT);
		}
		return false; // 登録失敗
	}

	//全削除
	void Clear()
	{
		for (int i = 0; i < m_iCellNum; i++)
		{
			if (!ppCellAry[i])	continue;

			OFT<T>* pOFT = ppCellAry[i]->pLatest;
			while (pOFT != nullptr)
			{
				OFT<T>* next_pOFT = pOFT->m_pNext;
				delete pOFT; // 生成したOFTオブジェクトを削除
				pOFT = next_pOFT;
			}
		}
	}

	//ある空間の親空間を取得
	Cell<T>* GetParentCell(Cell<T>* cell)
	{
		// ルート空間ならnullptrでreturn
		if (cell->m_iIndex == 0)
			return nullptr;

		int parentIndex = (cell->m_iIndex - 1) >> 3;
		return ppCellAry[parentIndex];
	}

	//ある点が属する最小空間のモートン番号
	int Get3DMortonNumber(float x, float y, float z)
	{
		// 点の座標を8分木空間内の座標に変換
		x = x - m_fLeft;
		y = y - m_fBottom;
		z = z - m_fFront;

		// 空間外の場合は-1でreturn
		if (x < 0 || m_fW < x)	return -1;
		//if (y < 0 || m_fH < y)	return -1;
		if (z < 0 || m_fD < z)	return -1;
		
		// yは空間内に
		if (y < 0)	y = 0;
		if (y > m_fH) y = m_fH;

		// 空間内の座標を最小空間の辺の長さで割る
		x = x / m_fUnit_W;
		y = y / m_fUnit_H;
		z = z / m_fUnit_D;

		// 座標をint型にキャストしてモートン番号を算出
		return Get3DMortonOrder(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
	}

	//登録空間の配列番号算出
	int GetLinerIndex(float pos_minX, float pos_maxX, float pos_minY, float pos_maxY, float pos_minZ, float pos_maxZ)
	{
		// オブジェクトを包むボックス2点のモートン番号
		int minMortonNomber = Get3DMortonNumber(pos_minX, pos_minY, pos_minZ);
		int maxMortonNomber = Get3DMortonNumber(pos_maxX, pos_maxY, pos_maxZ);

		if (minMortonNomber < 0 || maxMortonNomber < 0)
			return -1;

		int XOR = minMortonNomber ^ maxMortonNomber;	// 2点のxor

		int level = m_uiLevel;	// 初めは最小空間レベルから
		int shift = 0;	// 右に動かすシフト数

		while (XOR != 0)
		{
			level--;	// 分割レベルを1つ下げる
			shift += 3; // 3bit分増やす
			XOR >>= 3;	// 3bitシフトさせて再チェック
		}
		
		// 分割レベル = level でのモートン番号
		int mortonNomber = minMortonNomber >> shift;

		// 線形空間配列の配列番号
		int index = (m_iPow[level] - 1) / 7 + mortonNomber;
		return index;
		//return (m_iPow[level] - 1) / 7 + mortonNomber;
	}

private:
	//3bitごとの分割関数
	int BitSeparateFor3D(int n)
	{
		n = (n | n << 8) & 0x0000f00f;
		n = (n | n << 4) & 0x000c30c3;
		n = (n | n << 2) & 0x00249249;
		return n;
	}

	//8分木モートン順序算出関数
	int Get3DMortonOrder(int x, int y, int z)
	{
		return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
	}

	//空間生成
	void CreateNewCell(int Elem)
	{
		// 引数の要素番号
		while (!ppCellAry[Elem])
		{
			// 指定の要素番号に空間を新規作成
			ppCellAry[Elem] = new Cell<T>;
			ppCellAry[Elem]->m_iIndex = Elem;
			// 親空間にジャンプ
			Elem = (Elem - 1) >> 3;
			if (Elem >= m_iCellNum) break;
		}
	}

public:
	static const unsigned int MAXLEVEL = 8;//分割レベル最大値
	int m_iPow[MAXLEVEL + 1] = {};//各レベルでの空間数
	int m_iCellNum = 0;//合計空間数
	Cell<T>** ppCellAry = nullptr;//線形空間配列

	float m_fLeft = 0;
	float m_fBottom = 0;
	float m_fFront = 0;
	float m_fW = 0;
	float m_fH = 0;
	float m_fD = 0;
	float m_fUnit_W = 0;
	float m_fUnit_H = 0;
	float m_fUnit_D = 0;
	unsigned int m_uiLevel = 0;
};