//换一批商品事件
let btn = document.getElementsByClassName("btn")[0];
btn.onclick =function() {
    let ul1 = document.getElementsByClassName("ul_1")[0];
    let state = getComputedStyle(ul1).display;
    let ul2 = document.getElementsByClassName("ul_2")[0];
    if(state == "none"){
        ul1.style.display = "block";
        ul2.style.display = "none";

    }else{
        ul2.style.display = "block";
        ul1.style.display = "none";
    }
}


//图片轮播
function img_show(){
    
    let i=1;
    let showimgindex;
    for(i=1;i<6;i++){
        let img = document.getElementById("img"+i);
        if(getComputedStyle(img).display=="inline"){
            //第i张图片已经显示了
            if(i==5){
                showimgindex=1;
            }else{
                showimgindex=i+1;
            }
            break;

        }
    }
    for(i=1;i<6;i++){
        let img = document.getElementById("img"+i);
        if(i==showimgindex){
            img.style.display = "inline";
        }else{
            img.style.display = "none";
        }
    }
}


//定时器实现轮播
setInterval(img_show,2000);