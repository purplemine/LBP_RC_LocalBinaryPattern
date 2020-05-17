use super::LBPBase::IBase;
extern crate image;
use image::{GrayImage, Luma, Pixel};

pub struct CircularLBP
{
    _radius : u32,
    _angel  : u32,
    _neighbor : u32,
}
impl IBase for CircularLBP
{
    fn run_mutable(&self, src : &GrayImage) -> GrayImage
    {
        let pi = std::f32::consts::PI;
        let (rows, cols) = src.dimensions();
        if rows < 2 * self._radius || cols < 2 * self._radius
        {            
            panic!("rows or cols is less than 2 * radius({})", self._radius);
        }
        let (t_rows, t_cols) : (u32, u32)= (rows - 2 * self._radius, cols - 2 * self._radius);
        let mut dst : GrayImage = GrayImage::new(t_rows, t_cols);  
                    
        let (mut rx, mut ry, mut tx, mut ty) : (f32, f32, f32, f32) = (0.0, 0.0, 0.0, 0.0);        
        let (mut x1, mut y1, mut x2, mut y2) : (i32, i32, i32, i32) = (0, 0, 0, 0);
        let (mut w1, mut w2, mut w3, mut w4) : (f32, f32, f32, f32) = (0.0, 0.0, 0.0, 0.0);

        let mut rotate_angle : u32 = 0;      
        for k in 0 .. self._neighbor
        {
            if k == 0
            {
                for i in 0 .. t_rows{
                    for j in 0 .. t_cols{
                        dst.get_pixel_mut(i, j).channels_mut()[0] = 0u8;
                    }
                }
            }
            rotate_angle = ((k + self._angel) % self._neighbor);
            let angles : f32 = (pi * (rotate_angle as f32) / (self._neighbor as f32)) as f32;
            let angle_cos : f32 = angles.cos();
            let angle_sin : f32 = angles.sin();

            rx = (self._radius as f32) * angle_cos;
            ry = (self._radius as f32) * angle_sin;

            x1 = rx.floor() as i32;
            x2 = rx.ceil() as i32;
            y1 = ry.floor() as i32;
            y2 = ry.ceil() as i32;

           

            tx = (rx - (x1 as f32)) as f32;
            ty = (ry - (y1 as f32)) as f32;
            w1 = (1.0 - tx) * (1.0 - ty);
            w2 = tx * (1.0 - ty);
            w3 = (1.0 - tx) * ty;
            w4 = tx * ty;

           

            for i in self._radius .. (rows - self._radius)
            {
                for j in self._radius .. (cols - self._radius)
                {
                    let t_i : u32 = i - self._radius;
                    let t_j : u32 = j - self._radius;
                    let src_center : f32 = src.get_pixel(i, j).channels()[0] as f32;
                    let src_edge : f32 =  (src.get_pixel((i as i32 + x1) as u32, (j as i32 + y1) as u32 ).channels()[0] as f32) * w1
                                        + (src.get_pixel((i as i32 + x1) as u32, (j as i32 + y2) as u32 ).channels()[0] as f32) * w2
                                        + (src.get_pixel((i as i32 + x2) as u32, (j as i32 + y1) as u32 ).channels()[0] as f32) * w3
                                        + (src.get_pixel((i as i32 + x2) as u32, (j as i32 + y2) as u32 ).channels()[0] as f32) * w4;
                    let dst_p = dst.get_pixel_mut(t_i, t_j).channels_mut();
                    if src_edge > src_center
                    {
                        dst_p[0] = (dst_p[0] << 1) ^ 1u8;
                    }
                    else
                    {
                        dst_p[0] = (dst_p[0] << 1) ^ 0u8;
                    }
                }
            }
        }
        return dst;
    }
}
impl CircularLBP
{
    pub fn new(r : u32, a : u32, n : u32) -> CircularLBP
    {
        return CircularLBP{_radius : r, _angel : a, _neighbor : n};
    }
}