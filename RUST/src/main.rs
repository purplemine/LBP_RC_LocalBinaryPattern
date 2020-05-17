mod LBPBase;
mod BasicLBP;
mod CircularLBP;
mod MultiscaleBlockLBP;
use image::{DynamicImage, GrayImage, RgbImage, Pixel};
use std::path::Path;
use LBPBase::IBase;

use std::time::Instant;

fn main() {
    println!("Hello, world!");
    test();
}

fn test()
{
    let start = Instant::now();
    let img : DynamicImage = image::open(&Path::new("./test.jpg")).unwrap();
    let record_open_img = start.elapsed().as_millis();
    
    if let DynamicImage::ImageRgb8(d3img) = img { // enum
        let src = split(&d3img);
        let str_basic_lbp = start.elapsed().as_millis();
        println!("Start process image, at {:?}", str_basic_lbp);
        
        let basic_lbp = BasicLBP::BasicLBP{};
        let dst_basick_lbp = basic_lbp.run_mutable(&src);
        dst_basick_lbp.save("result_basic.jpg").unwrap();
        let tar_basic_lbp = start.elapsed().as_millis();
        println!("processed basic lbp image as {:?}", tar_basic_lbp);

        let cir_lbp = CircularLBP::CircularLBP::new(3, 0, 8);
        let dst_cir_lbp = cir_lbp.run_mutable(&src);
        dst_cir_lbp.save("result_circular.jpg").unwrap();
        let tar_cir_lbp = start.elapsed().as_millis();
        println!("processed circular lbp image as {:?}", tar_cir_lbp);

        let mcb_lbp = MultiscaleBlockLBP::MultiscaleBlockLBP::new(3, 0);
        let dst_mcb_lbp = mcb_lbp.run_mutable(&src);
        dst_mcb_lbp.save("result_multiscale_block.jpg").unwrap();
        let tar_mcb_lbp = start.elapsed().as_millis();
        println!("processed multiscale_block lbp image as {:?}", tar_mcb_lbp);

    }   

}
fn split(img : &RgbImage) -> GrayImage
{
    let (rows, cols) : (u32, u32) = img.dimensions();

    let mut dst = GrayImage::new(rows, cols);
    for i in 0 .. rows
    {
        for j in 0 .. cols
        {
            let mut dst_p = dst.get_pixel_mut(i, j).channels_mut();
            let src_p = img.get_pixel(i, j).channels();
            dst_p[0] = ((src_p[0] as u32) + (src_p[1] as u32)+ ( src_p[2] as u32) / 3) as u8;
        }
    }
    return dst;
}
