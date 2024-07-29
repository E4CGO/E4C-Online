#pragma once

// writer Matsumura

template< class T>
class Liner8TreeManager		// ���`8���ؔz��
{
public:
	template< class T>
	struct Cell;

	template< class T>
	struct OFT	//OBJECT_FOR_TREE�\����
	{
		Cell<T>* m_pCell = nullptr;//�o�^���
		T* m_pObject = nullptr;//����ΏۃI�u�W�F�N�g
		OFT<T>* m_pPre = nullptr;//�O��OFT�I�u�W�F�N�g
		OFT<T>* m_pNext = nullptr;//����OFT�I�u�W�F�N�g
		bool Remove()//���X�g���珜�O
		{
			// ���łɈ�E���Ă��鎞�͏����I��
			if (!m_pCell)
				return false;

			// ������o�^���Ă����ԂɎ��g��ʒm
			if (!m_pCell->OnRemove(this))
				return false;

			// ��E����
			// �O��̃I�u�W�F�N�g�����т���
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
		bool Clear();//�P���폜
	};

	template< class T>
	struct Cell
	{
		OFT<T>* pLatest = nullptr;//�ŐVOFT�I�u�W�F�N�g�̃A�h���X
		int m_iIndex = 0;//���̋�Ԃ̔z��ԍ�
		bool Push(OFT<T>* pOFT)//OFT�I�u�W�F�N�g�o�^
		{
			if (!pOFT)
				return false;

			if (pOFT->m_pCell == this)	// �Q�d�o�^�`�F�b�N
				return false;

			if (!pLatest)
			{
				pLatest = pOFT;	// ��ԂɐV�K�o�^
			}
			else
			{
				// �ŐVOFT�I�u�W�F�N�g���X�V
				pOFT->m_pNext = pLatest;
				pLatest->m_pPre = pOFT;
				pLatest = pOFT;
			}

			// �o�^����OFT�I�u�W�F�N�g�ɂ��̋�Ԃ�o�^
			pOFT->m_pCell = this;
			return true;
		}
		bool OnRemove(OFT<T>* pOFT)//�ŐVOFT�I�u�W�F�N�g�����O
		{
			if (!pOFT)
				return false;

			if (pOFT == pLatest)
			{
				pLatest = pOFT->m_pNext;	// �ŐVOFT�I�u�W�F�N�g�����炷
			}
			return true;
		}
	};

	//���`8���ؔz����\�z
	bool Initialize(unsigned int Level, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		// �ݒ�ō����x���ȏ�̋�Ԃ͍��Ȃ�
		if (Level > MAXLEVEL)
			return false;

		// �e���x���ł̋�Ԑ����Z�o
		m_iPow[0] = 1;
		for (int i = 1; i < MAXLEVEL + 1; i++)
		{
			m_iPow[i] = m_iPow[i - 1] * 8;
		}

		// Level�񕪊��i0��_�j�̔z��쐬
		m_iCellNum = (m_iPow[Level] * 8 - 1) / 7;
		ppCellAry = new Cell<T>*[m_iCellNum];
		ZeroMemory(ppCellAry, sizeof(Cell<T>*) * m_iCellNum);

		// �L���̈��o�^
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

	//�I����
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
					delete pOFT->m_pObject; // OFT���̃I�u�W�F�N�g���폜
					delete pOFT; // ��������OFT�I�u�W�F�N�g���폜
					pOFT = next_pOFT;
				}

				delete ppCellAry[i]; // ����������Ԃ��폜
			}
		}

		delete[] ppCellAry;	// ���`��Ԕz��폜
	}

	//�I�u�W�F�N�g�o�^
	bool Regist(float pos_minX, float pos_maxX, float pos_minY, float pos_maxY, float pos_minZ, float pos_maxZ, T* pObj)
	{
		// �I�u�W�F�N�g�̋��E�͈͂���o�^���[�g���ԍ����Z�o
		int Elem = GetLinerIndex(pos_minX, pos_maxX, pos_minY, pos_maxY, pos_minZ, pos_maxZ);

		if (Elem < 0)
			return false;// �o�^���s

		OFT<T>* pOFT = new OFT<T>;
		pOFT->m_pObject = pObj;

		if (Elem < m_iCellNum) {
			// ��Ԃ������ꍇ�͐V�K�쐬
			if (!ppCellAry[Elem])
				CreateNewCell(Elem);
			return ppCellAry[Elem]->Push(pOFT);
		}
		return false; // �o�^���s
	}

	//�S�폜
	void Clear()
	{
		for (int i = 0; i < m_iCellNum; i++)
		{
			if (!ppCellAry[i])	continue;

			OFT<T>* pOFT = ppCellAry[i]->pLatest;
			while (pOFT != nullptr)
			{
				OFT<T>* next_pOFT = pOFT->m_pNext;
				delete pOFT; // ��������OFT�I�u�W�F�N�g���폜
				pOFT = next_pOFT;
			}
		}
	}

	//�����Ԃ̐e��Ԃ��擾
	Cell<T>* GetParentCell(Cell<T>* cell)
	{
		// ���[�g��ԂȂ�nullptr��return
		if (cell->m_iIndex == 0)
			return nullptr;

		int parentIndex = (cell->m_iIndex - 1) >> 3;
		return ppCellAry[parentIndex];
	}

	//����_��������ŏ���Ԃ̃��[�g���ԍ�
	int Get3DMortonNumber(float x, float y, float z)
	{
		// �_�̍��W��8���؋�ԓ��̍��W�ɕϊ�
		x = x - m_fLeft;
		y = y - m_fBottom;
		z = z - m_fFront;

		// ��ԊO�̏ꍇ��-1��return
		if (x < 0 || m_fW < x)	return -1;
		//if (y < 0 || m_fH < y)	return -1;
		if (z < 0 || m_fD < z)	return -1;
		
		// y�͋�ԓ���
		if (y < 0)	y = 0;
		if (y > m_fH) y = m_fH;

		// ��ԓ��̍��W���ŏ���Ԃ̕ӂ̒����Ŋ���
		x = x / m_fUnit_W;
		y = y / m_fUnit_H;
		z = z / m_fUnit_D;

		// ���W��int�^�ɃL���X�g���ă��[�g���ԍ����Z�o
		return Get3DMortonOrder(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
	}

	//�o�^��Ԃ̔z��ԍ��Z�o
	int GetLinerIndex(float pos_minX, float pos_maxX, float pos_minY, float pos_maxY, float pos_minZ, float pos_maxZ)
	{
		// �I�u�W�F�N�g���ރ{�b�N�X2�_�̃��[�g���ԍ�
		int minMortonNomber = Get3DMortonNumber(pos_minX, pos_minY, pos_minZ);
		int maxMortonNomber = Get3DMortonNumber(pos_maxX, pos_maxY, pos_maxZ);

		if (minMortonNomber < 0 || maxMortonNomber < 0)
			return -1;

		int XOR = minMortonNomber ^ maxMortonNomber;	// 2�_��xor

		int level = m_uiLevel;	// ���߂͍ŏ���ԃ��x������
		int shift = 0;	// �E�ɓ������V�t�g��

		while (XOR != 0)
		{
			level--;	// �������x����1������
			shift += 3; // 3bit�����₷
			XOR >>= 3;	// 3bit�V�t�g�����čă`�F�b�N
		}
		
		// �������x�� = level �ł̃��[�g���ԍ�
		int mortonNomber = minMortonNomber >> shift;

		// ���`��Ԕz��̔z��ԍ�
		int index = (m_iPow[level] - 1) / 7 + mortonNomber;
		return index;
		//return (m_iPow[level] - 1) / 7 + mortonNomber;
	}

private:
	//3bit���Ƃ̕����֐�
	int BitSeparateFor3D(int n)
	{
		n = (n | n << 8) & 0x0000f00f;
		n = (n | n << 4) & 0x000c30c3;
		n = (n | n << 2) & 0x00249249;
		return n;
	}

	//8���؃��[�g�������Z�o�֐�
	int Get3DMortonOrder(int x, int y, int z)
	{
		return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
	}

	//��Ԑ���
	void CreateNewCell(int Elem)
	{
		// �����̗v�f�ԍ�
		while (!ppCellAry[Elem])
		{
			// �w��̗v�f�ԍ��ɋ�Ԃ�V�K�쐬
			ppCellAry[Elem] = new Cell<T>;
			ppCellAry[Elem]->m_iIndex = Elem;
			// �e��ԂɃW�����v
			Elem = (Elem - 1) >> 3;
			if (Elem >= m_iCellNum) break;
		}
	}

public:
	static const unsigned int MAXLEVEL = 8;//�������x���ő�l
	int m_iPow[MAXLEVEL + 1] = {};//�e���x���ł̋�Ԑ�
	int m_iCellNum = 0;//���v��Ԑ�
	Cell<T>** ppCellAry = nullptr;//���`��Ԕz��

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