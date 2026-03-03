const SCRIPT = "../cgi-bin/comment.py";
const form = document.getElementById("postComment");

form.addEventListener("submit", function (e) {
    e.preventDefault();

    const formData = new FormData(form);

    fetch(SCRIPT, {
        method: "POST",
        body: formData
    })
    .then(res => {
        if (res.status === 200) {
            return res.text();
        } else {
            throw new Error("Comment posting failed");
        }
    })
    .then(responseText => {
        // Create a Blob with the response text
        const blob = new Blob([responseText], { type: "text/html" });
		const text = await blob.text();
        // Create an object URL for the Blob
        const url = URL.createObjectURL(blob);
        // Open the URL in a new tab
        window.open(url, "_blank");
        // Optionally, reload the main page
        window.location.reload();
    })
    .catch(err => {
        console.log(err);
        alert("Error posting comment");
    });
});
