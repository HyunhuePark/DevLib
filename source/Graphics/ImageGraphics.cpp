#include "../../include/Graphics/ImageGraphics.hpp"

#include "TextASCII.hpp"
#include <cmath>

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
#include <cstring>
#endif

namespace DevLib {
	namespace Graphics {
		namespace ImageGraphics {
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			
			inline bool CheckInRect(int32_t &x1, int32_t &y1, int32_t &x2, int32_t &y2, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY)
			{
				bool bRet = true;
				double p[4], q[4];
				p[0] = -(x2 - x1); p[1] = -p[0]; p[2] = -(y2 - y1); p[3] = -p[2];
				q[0] = x1 - minX; q[1] = maxX - x1; q[2] = y1 - minY; q[3] = maxY - y1;

				double u1 = 0, u2 = 1;
				for (int32_t i = 0; i < 4; i++)
				{
					if (p[i] == 0)
					{
						if (q[i] < 0)
						{
							// 직선이 사각형 밖에 있습니다.
							bRet = false;
						}
					}
					else
					{
						const double t = q[i] / p[i];
						if (p[i] < 0 && u1 < t)
							u1 = t;
						else if (p[i] > 0 && u2 > t)
							u2 = t;
					}
				}

				if (u1 > u2)
				{
					// 직선이 사각형 밖에 있습니다.
					bRet = false;
				}

				// 직선의 끝점을 사각형의 경계와의 교점으로 대체합니다.
				const auto nx1 = static_cast<int32_t>(x1 + u1 * (x2 - x1));
				const auto ny1 = static_cast<int32_t>(y1 + u1 * (y2 - y1));
				const auto nx2 = static_cast<int32_t>(x1 + u2 * (x2 - x1));
				const auto ny2 = static_cast<int32_t>(y1 + u2 * (y2 - y1));

				x1 = nx1;
				x2 = nx2;
				y1 = ny1;
				y2 = ny2;

				return bRet;
			}

			//////////////////////////////////////////////////////////////////////////
			// Point
			void DrawPoint(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, CImageColor& color)
			{
				if (x < imgWidth && x >= 0 && y < imgHeight && y >= 0)
				{
					//if( nChannels == 1 ) img[ y * widthStep + x ] =  color.b;
					//else if( nChannels == 3 )
					//{
					//	img[ y * widthStep + x * nChannels + 0] =  color.b;
					//	img[ y * widthStep + x * nChannels + 1] =  color.g;
					//	img[ y * widthStep + x * nChannels + 2] =  color.r;
					//}
					memcpy(&img[y * widthStep + x * nChannels], color.data(), nChannels);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Line
#if defined(_MSC_VER)
#pragma warning(disable :4244)
#endif
			void DrawLineDDA(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor& color)
			{
				if (CheckInRect(x1, y1, x2, y2, 0, 0, imgWidth, imgHeight))
				{
					float_t x_inc, y_inc;

					auto dx = static_cast<float>(x2 - x1);
					auto dy = static_cast<float>(y2 - y1);

					if (dx >= 0)
					{
						x_inc = nChannels;
					}
					else
					{
						x_inc = -nChannels;
						dx = -dx;
					}

					if (dy >= 0)
					{
						y_inc = widthStep;
					}
					else
					{
						y_inc = -widthStep;
						dy = -dy;
					}

					auto x = static_cast<float_t>(x1);
					auto y = static_cast<float_t>(y1);

					// offset�� �ʱ�ȭ
					int off = widthStep * y1 + x1 * nChannels;


					if (dx > dy)
					{
						const float_t m = fabs(static_cast<float_t>((y2 - y1) / (x2 - x1)));

						for (int i = 0; i < dx; ++i)
						{
							memcpy(&img[off], color.data(), nChannels);

							off += x_inc;

							if (y > static_cast<float_t>(y1))
							{
								off += y_inc;
								++y1;
							}
							y += m;
						}
					}
					else
					{
						const float_t m = fabs(static_cast<float_t>(x2 - x1) / (y2 - y1));

						for (int i = 0; i < dy; ++i)
						{
							memcpy(&img[off], color.data(), nChannels);

							off += y_inc;

							if (x > static_cast<float_t>(x1))
							{
								off += x_inc;
								++x1;
							}
							x += m;
						}
					}
				}
			}

			void DrawLineBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				int32_t x1, int32_t y1, int32_t x2, int32_t y2, CImageColor& color)
			{
				if (CheckInRect(x1, y1, x2, y2, 0, 0, imgWidth, imgHeight))
				{
					int32_t x, y, i;

					const int32_t dx = x2 - x1;
					const int32_t dy = y2 - y1;
					const int32_t dx1 = abs(dx);
					const int32_t dy1 = abs(dy);

					int px = 2 * dy1 - dx1;
					int py = 2 * dx1 - dy1;
					if (dy1 <= dx1)
					{
						int xe;
						if (dx >= 0)
						{
							x = x1;
							y = y1;
							xe = x2;
						}
						else
						{
							x = x2;
							y = y2;
							xe = x1;
						}

						DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x, y, color);
						for (i = 0; x < xe; i++)
						{
							x = x + 1;
							if (px < 0)
							{
								px = px + 2 * dy1;
							}
							else
							{
								if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
								{
									y = y + 1;
								}
								else
								{
									y = y - 1;
								}
								px = px + 2 * (dy1 - dx1);
							}

							DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x, y, color);
						}
					}
					else
					{
						int ye;
						if (dy >= 0)
						{
							x = x1;
							y = y1;
							ye = y2;
						}
						else
						{
							x = x2;
							y = y2;
							ye = y1;
						}

						DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x, y, color);
						for (i = 0; y < ye; i++)
						{
							y = y + 1;
							if (py <= 0)
							{
								py = py + 2 * dx1;
							}
							else
							{
								if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
								{
									x = x + 1;
								}
								else
								{
									x = x - 1;
								}
								py = py + 2 * (dx1 - dy1);
							}

							DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x, y, color);
						}
					}
				}
			}

			inline void DrawHorizenLine(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& length, CImageColor& color)
			{
				// for( int i = x; i < x + imgWidth ; i++ ) DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, i, y, color);
				int _length = length;
				int _x = x;

				if (_x < 0)
				{
					_length = length + x;
					_x = 0;
				}

				if (_x < imgWidth && _x >= 0 && y < imgHeight && y >= 0)
				{
					if (_x + _length >= imgWidth) _length = imgWidth - _x;
					const int32_t pos = y * widthStep + _x * nChannels;
					int off = 0;
					for (int i = 0; i < _length; i++)
					{
						memcpy(&img[pos + off], color.data(), nChannels);
						off += nChannels;
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Circle
			void DrawCircleBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& radius, CImageColor& color)
			{
				int x_coor = 0;
				int y_coor = radius;
				int p_value = 3 - 2 * radius;
				int i = 0;
				while (x_coor < y_coor)
				{
					i++;
					PloatCircle(img, imgWidth, imgHeight, nChannels, widthStep, x_center, y_center, x_coor, y_coor, color);
					if (p_value < 0)
					{
						p_value += 4 * x_coor + 6;
					}
					else
					{
						p_value += 4 * (x_coor - y_coor) + 10;
						y_coor--;
					}
					x_coor++;
				}
				if (x_coor == y_coor)
					PloatCircle(img, imgWidth, imgHeight, nChannels, widthStep, x_center, y_center, x_coor, y_coor, color);
			}



			void DrawFillCircleBresenham(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& radius, CImageColor& color)
			{
				int x_coor = 0;
				int y_coor = radius;
				int p_value = 3 - 2 * radius;
				int i = 0;

				while (x_coor < y_coor)
				{
					i++;
					FillPloatCircle(img, imgWidth, imgHeight, nChannels, widthStep, x_center, y_center, x_coor, y_coor, color);
					if (p_value < 0)
					{
						p_value += 4 * x_coor + 6;
					}
					else
					{
						p_value += 4 * (x_coor - y_coor) + 10;
						y_coor--;
					}
					x_coor++;
				}
				if (x_coor == y_coor)
					FillPloatCircle(img, imgWidth, imgHeight, nChannels, widthStep, x_center, y_center, x_coor, y_coor, color);
			}


			//////////////////////////////////////////////////////////////////////////
			// 
			void DrawRect(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& rectWidth, const int32_t& rectHeight, CImageColor& color)
			{
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x, y, x + rectWidth, y, color);
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x, y + rectHeight, x + rectWidth, y + rectHeight, color);
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x, y, x, y + rectHeight, color);
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x + rectWidth, y, x + rectWidth, y + rectHeight, color);
			}


			void DrawFillRect(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& rectWidth, const int32_t& rectHeight, CImageColor& color)
			{
				int cy = 0;
				for (int yInc = 0; yInc < rectHeight; yInc++)
				{
					cy = y + yInc;
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x, cy, rectWidth, color);
				}

			}

			void PloatCircle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& x_coor, const int32_t& y_coor, CImageColor& color)
			{
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center + x_coor, y_center + y_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center - x_coor, y_center + y_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center + x_coor, y_center - y_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center - x_coor, y_center - y_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center + y_coor, y_center + x_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center - y_coor, y_center + x_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center + y_coor, y_center - x_coor, color);
				DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x_center - y_coor, y_center - x_coor, color);
			}


			void FillPloatCircle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x_center, const int32_t& y_center, const int32_t& x_coor, const int32_t y_coor, CImageColor& color)
			{
				DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x_center - x_coor, y_center + y_coor, (x_center + x_coor) - (x_center - x_coor), color);
				DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x_center - y_coor, y_center + x_coor, (x_center + y_coor) - (x_center - y_coor), color);
				DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x_center - y_coor, y_center - x_coor, (x_center + y_coor) - (x_center - y_coor), color);
				DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x_center - x_coor, y_center - y_coor, (x_center + x_coor) - (x_center - x_coor), color);
			}

			void DrawString(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& x, const int32_t& y, const int32_t& fontSize, const char* str, const uint32_t strCount, CImageColor& color)
			{
				int32_t sx = x;
				int32_t sy = y;
				int32_t cx, cy;
				int32_t _x, _y;

				for (uint32_t i = 0; i < strCount; i++)
				{
					const int32_t pos = static_cast<uint8_t>(str[i]);

					if (str[i] == '\n')
					{
						sx = x;
						sy += fontSize + 2;
					}
					else if (str[i] == '\r')
					{
						sx = x;
					}
					else if (str[i] == 32)
					{
						sx += (fontSize / 2);
					}
					else if (TextASCII[pos].height && TextASCII[pos].width)
					{
						const int32_t width = fontSize / 20.0 * static_cast<double_t>(TextASCII[pos].width);
						const int32_t height = fontSize / 20.0 * static_cast<double_t>(TextASCII[pos].height);

						const double_t ratioX = static_cast<double_t>(TextASCII[pos].width) / width;
						const double_t ratioY = static_cast<double_t>(TextASCII[pos].height) / height;

						for (_y = 0; _y < height; _y++)
						{
							for (_x = 0; _x < width; _x++)
							{
								cx = static_cast<int32_t>(ratioX * _x);
								cy = static_cast<int32_t>(ratioY * _y);

								if (TextASCII[pos].data[cy][cx]) DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, sx + _x, sy + _y, color);
							}
						}

						if (fontSize < 20)
						{
							cy = TextASCII[pos].height - 1;
							for (_x = 0; _x < width; _x++)
							{
								cx = static_cast<int32_t>(ratioX * _x);

								if (TextASCII[pos].data[cy][cx]) DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, sx + _x, sy + _y, color);
							}
						}

						sx += width + 1;
					}
				}
			}


			void DrawTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x1, const int32_t& y1, const int32_t& x2, const int32_t& y2, const int32_t& x3, const int32_t& y3, CImageColor& color)
			{
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x1, y1, x2, y2, color);
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x2, y2, x3, y3, color);
				DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep, x3, y3, x1, y1, color);
			}

			void DrawEllipse(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, CImageColor& color)
			{
				int32_t x_coor = 0;
				int32_t y_coor = height;
				const long xone_squ = width * width;
				const long xtwo_squ = xone_squ << 1;
				const long yone_squ = height * height;
				const long ytwo_squ = yone_squ << 1;


				long delta = yone_squ - xone_squ * height + (xone_squ >> 2);
				long dx = 0;
				long dy = xtwo_squ * height;

				while (dx < dy)
				{
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x + x_coor, y + y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y + y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x + x_coor, y - y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y - y_coor, color);

					if (delta > 0)
					{
						y_coor--;
						dy -= xtwo_squ;
						delta -= dy;
					}
					x_coor++;
					dx += ytwo_squ;
					delta += yone_squ + dx;
				}

				delta += (3 * (xone_squ - yone_squ) / 2 - (dx + dy) / 2);

				while (y_coor >= 0)
				{
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x + x_coor, y + y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y + y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x + x_coor, y - y_coor, color);
					DrawPoint(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y - y_coor, color);

					if (delta < 0)
					{
						x_coor++;
						dx += ytwo_squ;
						delta += dx;
					}
					y_coor--;
					dy -= xtwo_squ;
					delta += xone_squ - dy;
				}
			}

			void DrawEllipseEx(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight,
				const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y,
				const int32_t& width, const int32_t& height, const double_t& rotate_deg, CImageColor& color,
				const int32_t& resolution)
			{
				const double heading = rotate_deg / 180 * 3.141592653589793;


				const double_t initX = width * sin(0);
				const double_t initY = height * cos(0);

				double_t prevX = initX * cos(heading) - initY * sin(heading);
				double_t prevY = initX * sin(heading) + initY * cos(heading);

				for (int i = 1; i <= resolution; i++)
				{
					const double_t tempX = width * sin((static_cast<double_t>(i) / resolution) * (3.141592653589793 * 2));
					const double_t tempY = height * cos((static_cast<double_t>(i) / resolution) * (3.141592653589793 * 2));

					const double_t curX = tempX * cos(heading) - tempY * sin(heading);
					const double_t curY = tempX * sin(heading) + tempY * cos(heading);

					DrawLineBresenham(img, imgWidth, imgHeight, nChannels, widthStep,
						static_cast<int32_t>(x + prevX), static_cast<int32_t>(y - prevY),
						static_cast<int32_t>(x + curX), static_cast<int32_t>(y - curY), color);

					prevX = curX;
					prevY = curY;
				}
			}

			void DrawFillEllipse(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x, const int32_t& y, const int32_t& width, const int32_t& height, CImageColor& color)
			{
				int32_t x_coor = 0;
				int32_t y_coor = height;
				const long xone_squ = width * width;
				const long xtwo_squ = xone_squ << 1;
				const long yone_squ = height * height;
				const long ytwo_squ = yone_squ << 1;


				long delta = yone_squ - xone_squ * height + (xone_squ >> 2);
				long dx = 0;
				long dy = xtwo_squ * height;

				while (dx < dy)
				{
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y + y_coor, x_coor * 2, color);
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y - y_coor, x_coor * 2, color);

					if (delta > 0)
					{
						y_coor--;
						dy -= xtwo_squ;
						delta -= dy;
					}
					x_coor++;
					dx += ytwo_squ;
					delta += yone_squ + dx;
				}

				delta += (3 * (xone_squ - yone_squ) / 2 - (dx + dy) / 2);

				while (y_coor >= 0)
				{
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y + y_coor, x_coor * 2, color);
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, x - x_coor, y - y_coor, x_coor * 2, color);

					if (delta < 0)
					{
						x_coor++;
						dx += ytwo_squ;
						delta += dx;
					}
					y_coor--;
					dy -= xtwo_squ;
					delta += xone_squ - dy;
				}
			}


			inline void FlatShadingTopTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& topX, const int32_t& topY, const int32_t& bottomX1, const int32_t& bottomY1, const int32_t& bottomX2, const int32_t& bottomY2, CImageColor& color)
			{
				int32_t lx = bottomX1, rx = bottomX2;
				int32_t ly = bottomY1, ry = bottomY2;

				if (lx > rx)
				{
					const int32_t tmpx = lx;
					const int32_t tmpy = ly;
					lx = rx;
					ly = ry;
					rx = tmpx;
					ry = tmpy;
				}

				float_t m1, m2;
				if (ly - topY == 0)
				{
					m1 = 0;
				}
				else
				{
					m1 = static_cast<float_t>(lx - topX) / static_cast<float_t>(ly - topY);
				}

				if (ry - topY == 0)
				{
					m2 = 0;
				}
				else
				{
					m2 = static_cast<float_t>(rx - topX) / static_cast<float_t>(ry - topY);
				}

				auto tmp_x1 = static_cast<float>(topX);
				auto tmp_x2 = tmp_x1;
				auto tmp_y = static_cast<float>(topY);

				const auto dy = static_cast<float>(ly - topY);
				for (uint32_t i = 0; static_cast<float>(i) <= dy; ++i)
				{
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, static_cast<int32_t>(floorf(tmp_x1 + 0.5f)), static_cast<int32_t>(tmp_y), static_cast<int32_t>(floorf((tmp_x2 - tmp_x1) + 0.5f)), color);

					tmp_x1 += m1;
					tmp_x2 += m2;
					tmp_y++;
				}
			}

			inline void FlatShadingBottomTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep,
				const int32_t& bottomX, const int32_t& bottomY, const int32_t& topX1, const int32_t& topY1, const int32_t& topX2, const int32_t& topY2, CImageColor& color)
			{
				if (bottomY == topY1 || bottomY == topY2)
				{
					return;
				}

				int32_t lx = topX1, rx = topX2;
				int32_t ly = topY1, ry = topY2;

				if (lx > rx)
				{
					const int32_t tempX = lx;
					const int32_t tempY = ly;
					lx = rx;
					ly = ry;
					rx = tempX;
					ry = tempY;
				}

				const float_t m1 = static_cast<float_t>(bottomX - lx) / static_cast<float_t>(bottomY - ly);
				const float_t m2 = static_cast<float_t>(bottomX - rx) / static_cast<float_t>(bottomY - ry);

				auto tmp_x1 = static_cast<float>(lx);
				auto tmp_x2 = static_cast<float>(rx);
				auto tmp_y = static_cast<float>(topY1);

				const auto dy = static_cast<float>(bottomY - topY1);

				for (uint32_t i = 0; static_cast<float>(i) <= dy; ++i)
				{
					DrawHorizenLine(img, imgWidth, imgHeight, nChannels, widthStep, static_cast<int32_t>(floorf(tmp_x1 + 0.5f)), static_cast<int32_t>(tmp_y), static_cast<int32_t>(floorf((tmp_x2 - tmp_x1) + 0.5f)), color);

					tmp_x1 += m1;
					tmp_x2 += m2;
					tmp_y++;
				}
			}


			void DrawFillTriangle(uint8_t* img, const int32_t& imgWidth, const int32_t& imgHeight, const int32_t& nChannels, const int32_t& widthStep, const int32_t& x1, const int32_t& y1, const int32_t& x2, const int32_t& y2, const int32_t& x3, const int32_t& y3, CImageColor& color)
			{
				int32_t temp_x;
				int32_t temp_y;
				int32_t _x1 = x1;
				int32_t _x2 = x2;
				int32_t _x3 = x3;
				int32_t _y1 = y1;
				int32_t _y2 = y2;
				int32_t _y3 = y3;

				if (_y1 > _y2)
				{
					temp_x = _x1;
					temp_y = _y1;

					_x1 = _x2;
					_y1 = _y2;

					_x2 = temp_x;
					_y2 = temp_y;
				}

				if (_y1 > _y3)
				{
					temp_x = _x1;
					temp_y = _y1;

					_x1 = _x3;
					_y1 = _y3;

					_x3 = temp_x;
					_y3 = temp_y;
				}

				if (_y2 > _y3)
				{
					temp_x = _x2;
					temp_y = _y2;

					_x2 = _x3;
					_y2 = _y3;

					_x3 = temp_x;
					_y3 = temp_y;
				}
				//---------------------------

				temp_y = _y2;
				float_t temp_m1;	// p1->p3
				if (_y3 - _y1 == 0)
				{
					temp_m1 = 0;
				}
				else
				{
					temp_m1 = static_cast<float_t>(_x3 - _x1) / static_cast<float_t>(_y3 - _y1);
				}

				const auto dy = static_cast<float_t>(_y2 - _y1);
				temp_x = _x1 + temp_m1 * dy;

				FlatShadingTopTriangle(img, imgWidth, imgHeight, nChannels, widthStep, _x1, _y1, _x2, _y2, temp_x, temp_y, color);
				FlatShadingBottomTriangle(img, imgWidth, imgHeight, nChannels, widthStep, _x3, _y3, _x2, _y2, temp_x, temp_y, color);
			}

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
		}
	}
}