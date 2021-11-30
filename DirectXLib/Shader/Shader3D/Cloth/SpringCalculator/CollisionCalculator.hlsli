//���ƒ����̓����蔻��
bool isHitSphereAndLine(float3 p1, float3 p2, float3 sph, float r) {
	float3 p1_sph = subtract(sph, p1);
	float3 p2_sph = subtract(sph, p2);
	float3 p1_p2 = subtract(p2, p1);
	if (dot(p1_sph, p1_p2) * dot(p2_sph, p1_p2) <= 0) {
		float theta = subtendedAngle(p1_sph, p1_p2);
		float d = abs(mul(p1_p2, p1_sph)) / distance(p2, p1);
		if (d <= r)return true;
	}
	return false;
}
//�O�p�`�̖@�������߂�
float3 createNormal(float3 p0,float3 p1,float3 p2) {
	float3 p0_p1 = subtract(p1, p0);
	float3 p0_p2 = subtract(p2, p0);
	float theta = subtendedAngle(p0_p1, p0_p2);
	return cross(p0_p1, p0_p2);
}
//�O�p�`�ƒ����̌�_
float3 CrossPolygonAndLine(float3 p1, float3 p2, float3 plane0, float3 plane1, float3 plane2) {
	//���_����@�������߂�
	float3 vn = createNormal(plane0, plane1, plane2);
	//���ʂƂ̋��������߂�
	float3 v1 = subtract(p1, plane0);
	float d1 = abs(dot(vn, v1)) / abs(vn);
	float3 v2 = subtract(p2, plane0);
	float d2 = abs(dot(vn, v2)) / abs(vn);
	//����������߂�B
	float ratio = d1 / (d1 + d2);
	//��_�����߂�
	float3 v3 = add(scale(v1, 1 - ratio), scale(v2, ratio));
	float3 p3 = add(plane0, v3);
	return p3;
}
//�O�p�`�Ɠ_�̓����蔻��
bool isHitPolygonAndPoint(float3 plane0, float3 plane1, float3 plane2, float3 p) {
	//���_�̊O�ς����߂�
	float3 p0_p1 = subtract(plane1, plane0);
	float3 p1_p = subtract(p, plane1);

	float3 p1_p2 = subtract(plane2, plane1);
	float3 p2_p = subtract(p, plane2);

	float3 p2_p0 = subtract(plane0, plane2);
	float3 p0_p = subtract(p, plane0);

	float3 A = cross(p0_p1, p1_p);
	float3 B = cross(p1_p2, p2_p);
	float3 C = cross(p2_p0, p0_p);
	float dot_AB = dot(A, B);
	float dot_AC = dot(A, C);
	if (dot_AB > 0 && dot_AC > 0)return true;
	return false;
}
//�O�p�`�ƒ����̓����蔻��
bool isHitPolygonAndLine(float3 p1, float3 p2, float3 plane0, float3 plane1, float3 plane2) {
	//��_�����߂�
	float3 p3 = CrossPolygonAndLine(p1, p2, plane0, plane1, plane2);
	//�|���S�����ɓ_�����邩�����߂�
	return isHitPolygonAndPoint(plane0, plane1, plane2, p3);
}
//���ʂƒ����̓����蔻��
bool isHitPlneAndLine(float3 p1, float3 p2, float3 plane0, float3 plane1, float3 plane2, float3 plane3) {
	//��ԉ������_(plane0�̑Ίp)��plane[2]�Ɋ��蓖�Ă�B����ȊO�Ŏc��̒��_(plane0�ȊO)�����蓖�Ă�
	float3 plane[3] = { plane1, plane2, plane3 };
	float dist_max = 0;
	for (int ite = 0; ite < 3; ite++) {
		float dist = distance(plane0, plane[ite]);
		if (dist > dist_max)dist_max = dist;
	}
	for (int ite = 0; ite < 3; ite++) {
		if (distance(plane0, plane[ite]) == dist_max) {
			float3 max_plane = plane[ite];
			plane[ite] = plane[2];
			plane[2] = max_plane;
			break;
		}
	}
	//���ʂ��|���S���ŕ����Ĕ���
	bool is_hit=false;
	is_hit = isHitPolygonAndLine(p1, p2, plane0, plane[0], plane[2]);
	if(!is_hit)is_hit = isHitPolygonAndLine(p1, p2, plane0, plane[1], plane[2]);
	return is_hit;
}