#pragma once

class Vertex {
public:
	explicit Vertex() { }
	~Vertex() { }

	void setPosition(float x, float y, float z) { mData[0] = x; mData[1] = y; mData[2] = z; }
	void setNormal(float x, float y, float z) { mData[3] = x; mData[4] = y; mData[5] = z; }

	void getPosition(float arr[3]) const {
		arr[0] = mData[0];
		arr[1] = mData[1];
		arr[2] = mData[2];
	}

	void getNormal(float* arr) const {
		arr[0] = mData[3];
		arr[1] = mData[4];
		arr[2] = mData[5];
	}

	bool cmp(const Vertex& v) const {
		for(int i = 0; i < 6; ++i) {
			if(abs(mData[i] - v.mData[i]) > 0.00001f)
				return false;
		}

		return true;
	}

private:
	float mData[6];
};
