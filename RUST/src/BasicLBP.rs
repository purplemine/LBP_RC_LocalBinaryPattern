use super::LBPBase::IBase;
extern crate image;
use image::{GrayImage, Luma, Pixel};

pub struct BasicLBP
{

}

impl IBase for BasicLBP
{
    fn run_mutable(&self, src : &GrayImage) -> GrayImage
    {
        // let dst : GrayImage = GrayImage::new(1,1);
        let radius : u32 = 1;
        let offset : u32 = 0;
        let (rows, cols) : (u32, u32) = src.dimensions();
        let (t_rows, t_cols) : (u32, u32) = (rows - 2 * radius, cols - 2 * radius);

        let mut dst = GrayImage::new(t_rows, t_cols);

        let neighbor : u32 = 8;
        let row_vec : [i32; 8] = [-1, -1, 0, 1, 1, 1, 0, -1];
        let col_vec : [i32; 8] = [0, 1, 1, 1, 0, -1, -1, -1];

        for k in 0 .. neighbor
        {
            if k == 0
            {
                for i in 0 .. t_rows
                {
                    for j in 0 .. t_cols
                    {
                        dst.get_pixel_mut(i, j).channels_mut()[0] = 0u8;
                    }
                }
            }

            for i in 0 .. t_rows
            {
                for j in 0 .. t_cols
                {
                    let cmp_x : u32 = (i as i32 + radius as i32 + row_vec[k as usize]) as u32;
                    let cmp_y : u32 = (j as i32 + radius as i32 + col_vec[k as usize]) as u32;
                    let src_cmp = src.get_pixel(cmp_x, cmp_y);
                    let src_cur = src.get_pixel(i + radius, j + radius);
                    
                    let dst_p = dst.get_pixel_mut(i, j).channels_mut();
                    if src_cur[0] > src_cmp[0]
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