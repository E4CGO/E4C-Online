#include "CollisionManager.h"

void CollisionManager::Contacts()
{
	for (int i = 0; i < items.size(); i++)
	{
		Collider* col1 = items[i];
		if (!col1->IsEnable()) continue;
		
		for (int j = i + 1; j < items.size(); j++)
		{
			Collider* col2 = items[j];
			if (!col2->IsEnable()) continue;

			if ((col1->GetHittableOBJ() & col2->GetOBJType()) || (col2->GetHittableOBJ() & col1->GetOBJType()))
			{
				HitResult hit;
				if (col1->Collision(col2, {}, hit))
				{
					if (col1->GetHittableOBJ() & col2->GetOBJType())
					{
						col1->OnCollision(col2);
					}
					if (col2->GetHittableOBJ() & col1->GetOBJType())
					{
						col2->OnCollision(col1);
					}
				}
			}
		}
	}
}
