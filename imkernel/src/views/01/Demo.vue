<script setup>
import { ref } from 'vue'
const count = ref(0)
console.log('count :>> ', count.value);
const handleClick = () => {
    count.value++
}
</script>

<template>
    <div>
        {{ count }}

        <button @click="handleClick">add</button>
    </div>
</template>

<style lang="css"></style>