function createCircle(color, size, id) {
        const $circle = $('<div></div>'); // Create a jQuery object

        $circle.css({
            width: size + "px",
            height: size + "px",
            backgroundColor: color,
            borderRadius: "50%",
            display: "inline-block",
            margin: "5px",
            cursor: "pointer"
        });

        $circle.on("click", () => {toDesk(color);});

        return $circle;
    }

    
function toDesk(color){

    o = colors[color]
    console.log(o)
   $.get(`/set/${o[0]}/${o[1]}/${o[2]}/255`)

}

states = $("#states").html().trim().split(/\s+/)
states.forEach((e)=>{$("#acts").append(createCircle(e,50))})

